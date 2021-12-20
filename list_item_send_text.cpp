#include <iostream>
#include <Windows.h>
#include <Commctrl.h>
#include <math.h>
#include <vector>
#include <fstream>

using namespace std;
HHOOK _hook;
HMENU menu;

void create_controls(HWND&);
void create_menu(HWND &);

HWND create_window(int,int);
void insert_line_into_list_view(HWND , string );

HWND list_view;
HWND text_box;

int window_width = 600;
int window_height = 450;
int padding = 20;
int left_padding = padding;
int right_padding = padding;
int top_padding = padding;
int bottom_padding = padding;
int x_right_66 = round(window_width - (window_width / 3));
const int help_id = 0x1000;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{

    HWND main_window = create_window(window_width,window_height);
    
    ShowWindow(main_window, nCmdShow);
    UpdateWindow(main_window);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return 0;
}


HWND get_hwnd_by_title_part(const char* title_part)
{
    HWND hwnd = NULL;
    while (hwnd = FindWindowEx(NULL, hwnd, NULL, NULL))
    {
        char title[256];
        GetWindowText(hwnd, title, 256);
        if (strstr(title, title_part))
        {
            return hwnd;
        }
    }
    return NULL;
}


class Items
{
    private:
        vector<string> items;
    public:

        //constructor
        Items(string filename)
        {
            get_lines_from_file(filename);
        }
        
        //no getline, use ancient c++
        void get_lines_from_file(string file_name)
        {
            ifstream file(file_name);
            string line;
            items.clear();
            while (getline(file, line))
            {
                items.push_back(line);
            }
        }
            
        vector<string> get_items_containing_string(string filter_query)
        {
            vector<string> filtered_items;
            for (int i = 0; i < items.size(); i++)
            {
                if (items[i].find(filter_query) != string::npos)
                {
                    filtered_items.push_back(items[i]);
                }
            }
            return filtered_items;
        }

        vector<string> get_items()
        {
            return items;
        }
        
};

//initialize items for wndproc
Items items_instance("list_item_send_text_lines.txt");





LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_CREATE:

         create_controls(hwnd);
         create_menu(hwnd);
        break;
        
    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
            //refresh
            case 2:
            {
                ListView_DeleteAllItems(list_view);
                //refresh
                items_instance.get_lines_from_file("list_item_send_text_lines.txt");
                vector<string> items = items_instance.get_items();
                for (int i = 0; i < items.size(); i++)
                {
                    insert_line_into_list_view(list_view, items[i]);
                }
                break;
            }
            //
            case 3:
            {
                break;
            }
            case 6:
            {
                //if changing the textbox
                if(HIWORD(wParam) == EN_CHANGE)
                {

                    char text_box_value[256];
                    GetWindowText(text_box, text_box_value, 256);
                    string text_box_value_string = text_box_value;
                    
                    ListView_DeleteAllItems(list_view);
                    vector<string> filtered_items = items_instance.get_items_containing_string(text_box_value_string);
                    for (int i = 0; i < filtered_items.size(); i++)
                    {
                        insert_line_into_list_view(list_view, filtered_items[i]);
                    }
                }
                break;
            }
            // help & about menu
            case help_id:
            {
                MessageBoxA(NULL, "This is a list of text.\n\nYou can add new items by typing them in the text box and pressing the add button.\nYou can delete items by selecting them and pressing the delete button.\nYou can copy items into the clipboard by selecting them and pressing the copy button. \n\nMade by Mark Mulcahy with Win32 C++ API\n\n(c) 2021", "Mark Mulcahy", MB_ICONEXCLAMATION);
                break;
            }
        }
            //add line from text box into the list_view variable
        break;
    case WM_LBUTTONDOWN:
        //print x & y from the client window
        // cout << "x: " << LOWORD(lParam) << endl;
        // cout << "y: " << HIWORD(lParam) << endl;
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    // on selecting an item in list_view, copy to clipboard
    case WM_NOTIFY:
    {
        NMHDR* nmhdr = (NMHDR*)lParam;
        if (nmhdr->hwndFrom == list_view && nmhdr->code == NM_CLICK)
        {
            //get selected text
            int selected_item_index = ListView_GetNextItem(list_view, -1, LVNI_SELECTED);
            if (selected_item_index == -1) break;
            char* item_text = new char[256];
            ListView_GetItemText(list_view, selected_item_index, 0, item_text, 256);
            string item_text_string = item_text;
            //copy to clipboard
            OpenClipboard(NULL);
            EmptyClipboard();
            HGLOBAL hg = GlobalAlloc(GMEM_MOVEABLE, item_text_string.size() + 1);
            if (!hg)
            {
                CloseClipboard();
                break;
            }
            memcpy(GlobalLock(hg), item_text_string.c_str(), item_text_string.size() + 1);
            GlobalUnlock(hg);
            SetClipboardData(CF_TEXT, hg);
            CloseClipboard();
        }         
        break;
    }
    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}




HWND create_window(int width, int height)
{
    HWND hwnd;
    WNDCLASSEX wc;
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = 0;
    wc.lpfnWndProc = WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = GetModuleHandle(NULL);
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    HBRUSH gray;
    gray = CreateSolidBrush(RGB(240, 240, 240));
    wc.hbrBackground = gray;
    wc.lpszMenuName = NULL;
    wc.lpszClassName = "Class";
    wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
    if (!RegisterClassEx(&wc))
    {
        MessageBox(NULL, "Window Registration Failed!", "Error!",
                   MB_ICONEXCLAMATION | MB_OK);
        return NULL;
    }    
    hwnd = CreateWindowEx(
        WS_EX_CLIENTEDGE,
        "Class", // name of the window class
        "Copy Line To Clipboard", // title of the window
        (WS_OVERLAPPEDWINDOW) & ~ (WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_THICKFRAME), // window style) ,
        CW_USEDEFAULT, CW_USEDEFAULT, width, height,
        NULL, NULL, GetModuleHandle(NULL), NULL);
    if (hwnd == NULL)
    {
        MessageBox(NULL, "Window Creation Failed!", "Error!",
                   MB_ICONEXCLAMATION | MB_OK);
        return NULL;
    }
    return hwnd;
}

void insert_line_into_list_view(HWND hwnd, string text)
{
    //get the last index of the list_view variable
    int last_index = ListView_GetItemCount(list_view);
    LVITEM lv;
    //insert a new vertical line in list view
    lv.mask = LVIF_TEXT;
    lv.iItem = last_index;
    lv.iSubItem = 0;
    lv.pszText = (LPSTR)text.c_str();
    //width is the list view width
    lv.cchTextMax = 50;
    ListView_InsertItem(hwnd, &lv);
}

void create_controls(HWND &hwnd)
{
    //font for buttons & label
    HFONT font = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
    

    //get client rect values
    RECT rect;
    GetClientRect(hwnd, &rect);
    int client_width = rect.right - rect.left;
    int client_height = rect.bottom - rect.top;
    //get the height of menu
    int menu_height = GetSystemMetrics(SM_CYMENU);
    client_height = client_height - menu_height;

    //put up a list view from x = 0 to x = x_right_66
    list_view = CreateWindowEx(
        WS_EX_CLIENTEDGE,
        WC_LISTVIEW,
        "Potential Comments",
        WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_SHOWSELALWAYS | LVS_ALIGNLEFT | LVS_SINGLESEL,
        left_padding,
        top_padding,
        x_right_66 - left_padding - right_padding,
        client_height - top_padding - bottom_padding,
        hwnd,
        (HMENU)1,
        GetModuleHandle(NULL),
        NULL);
    
    //grid with full click. For the full row select to happen, the list view must be in report mode AND with LVS_EX_GRIDLINES
    ListView_SetExtendedListViewStyle(list_view, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
    //font is the "System" font
    SendMessage(list_view, WM_SETFONT, (WPARAM)CreateFont(0, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, "System"), TRUE);

    //add in a column
    LVCOLUMN lvc;
    lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
    lvc.fmt = LVCFMT_LEFT;
    //get the width of list view
    RECT list_view_rect;
    GetWindowRect(list_view, &list_view_rect);
    // wish there was a better way than -4 
    int list_view_width = list_view_rect.right - list_view_rect.left - 4;
    lvc.cx = list_view_width;
    lvc.pszText = (LPSTR)"Click on row to copy to clipboard";
    lvc.iSubItem = 0;
    ListView_InsertColumn(list_view, 0, &lvc);

    //insert lines in a for loop in file named "list_item_send_text_lines.txt"
    vector<string> items = items_instance.get_items();

    //if items has nothing
    if (items.size() == 0)
    {
        //default if file is not found
        insert_line_into_list_view(list_view, "Hi");
        insert_line_into_list_view(list_view, "Hello");
        insert_line_into_list_view(list_view, "Goodbye");
        insert_line_into_list_view(list_view, "My name is Slim Shady");
        insert_line_into_list_view(list_view, "I like to dance");
        insert_line_into_list_view(list_view, "I like to party");
        insert_line_into_list_view(list_view, "I like to sing");
        insert_line_into_list_view(list_view, "I like to sleep");
        insert_line_into_list_view(list_view, "Who is your daddy");
        insert_line_into_list_view(list_view, "Who is your mother");
        insert_line_into_list_view(list_view, "I like to be with you");
        insert_line_into_list_view(list_view, "I like to eat");
        insert_line_into_list_view(list_view, "I like to travel");
        insert_line_into_list_view(list_view, "Do you like me");
        insert_line_into_list_view(list_view, "I like to be alone");
    }
    else
    {
        for (int i = 0; i < items.size(); i++)
        {
            insert_line_into_list_view(list_view, items[i]);
        }
    }




    //////////////////
    // BUTTONS
    //////////////////
    int button_height = 30;
    //button with devel mode
    HWND refresh = CreateWindowEx(
        0,
        "BUTTON",
        "Refresh",
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | WS_BORDER,
        x_right_66, 
        top_padding, //
        client_width - x_right_66 - right_padding,
        button_height, // 
        hwnd,
        (HMENU)2,
        GetModuleHandle(NULL),
        NULL);
    HWND disabled_button = CreateWindowEx(
        0,
        "BUTTON",
        "Disabled",
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | WS_BORDER | WS_DISABLED,
        x_right_66,
        top_padding * 2 + button_height * 1,
        client_width - x_right_66 - right_padding,
        button_height,
        hwnd,
        (HMENU)3,
        GetModuleHandle(NULL),
        NULL);
    //paste button is default for tabbed
    SetFocus(refresh);
    //set fonts into a Default vista font
    SendMessage(refresh, WM_SETFONT, (WPARAM)font, TRUE);
    SendMessage(disabled_button, WM_SETFONT, (WPARAM)font, TRUE);


    //////////////////
    // TEXT BOX
    //////////////////
    //no left padding here
    int text_box_section_x = x_right_66;
    int text_box_section_y = round(client_height * .60); //place the y coordinate at 66% of the client height
    text_box = CreateWindowEx(
        WS_EX_CLIENTEDGE,
        "EDIT",
        "",
        //word wrap & no scroll 
        WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE | BS_GROUPBOX | ES_WANTRETURN,
        text_box_section_x, //x
        text_box_section_y, //y
        client_width - text_box_section_x - right_padding, //width
        client_height - text_box_section_y - bottom_padding, // height
        hwnd,
        (HMENU)6,
        GetModuleHandle(NULL),
        NULL);
    int label_height = 20;
    //label
    HWND text_box_label = CreateWindowEx(
        0, // no extended styles
        "STATIC",
        "Filter",
        //not embossed
        WS_CHILD | WS_VISIBLE | SS_LEFT,
        text_box_section_x,
        text_box_section_y - label_height,
        client_width - text_box_section_x - right_padding, // width
        label_height,                                        // height
        hwnd,
        (HMENU)6,
        GetModuleHandle(NULL),
        NULL);
    //make the font of the button into small Microsoft Sans Serif like in Windows 95
    SendMessage(text_box_label, WM_SETFONT, (WPARAM)font, TRUE);
}

void create_menu(HWND &hwnd)
{
    //////////////////
    // MENU
    //////////////////
    //create the menu
    HMENU hMenu = CreateMenu();
    HMENU hHelpMenu = CreateMenu();

    AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hHelpMenu, "&Help");

    AppendMenu(hHelpMenu, MF_STRING, help_id, "&About");

    SetMenu(hwnd, hMenu);


}
