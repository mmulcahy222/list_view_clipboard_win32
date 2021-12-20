Someone really knows Windows & C++ when they work on Desktop applications in Win32, without external libraries like QT (where you probably have to pay to use it, or use to all the features).

This upped my game and got me very familiarized with Microsoft MSDN.

By knowing C++, one can code some advanced applications involving chips, hard drives, data centers, AWS Hyperplane, routing protocols, and anything deep in the low level. 

By knowing Win32, one gets to level up their system administrator skills and become very valuable to an organization. Win32 can do tasks far quicker than Powershell in my estimation. Programs run rapidly. 

This amazing program/app allows me to grow on both silos, as well as making something that has great use to me.

A "list view" control shows up on the side. The list view control has many options according to the MSDN site at [https://docs.microsoft.com/en-us/windows/win32/controls/list-view-window-styles]

Note the old school "System" font being used. I miss it, I wouldn't have minded using it temporary as a coding font, but there's no TrueType version of it online (I don't want it that bad to convert it)

An LVCOLUMN structure attaches to a list created by CreateWindowEx, who's second parameter is WC_LISTVIEW to designate it as a List View control.

There are also extended styles to the ListView that could only be set if the ListView was sent the 2nd parameter, which is sent as a bit with BITWISE OR operator known as LVS_REPORT, which is the last bit in that large parameter in CreateWindow which is a DWORD (Double Word) which is 32 bits, representing 32 different styles (mixed and matched).

![](images/list_view_program.gif)

The extended style is the LVS_EX_FULLROWSELECT, which selects the row when clicking on the list view. This is desired behavior because you can click on the row to select it, rathere than clicking on the words itself. To set this style, it's also necessary to add in the extended style of LVS_EX_GRIDLINES as well for LVS_EX_FULLROWSELECT to work.

The code will look like this:

ListView_SetExtendedListViewStyle(list_view, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES); 

LVS_EX_GRIDLINES is the first bit of the BITWISE OR operator of that parameter, and LVS_EX_FULLROWSELECT is 0x20, which means it's the 6th bit of the whole DWORD looking like 00000000000000000000000000100001

The contents of the list view are a list of lines that are written in a TEXT file in the same directory that the .cpp & .exe file were on. One the row is selected, it saves the text in the clipboard.

The way this is done is in the WM_NOTIFY case in the Windows Procedure. If the notification comes from a list_view AND if the code is NM_CLICK, it will save the text in the clipboard, so nobody has to type lines to people anymore by hand.

The original program had the Add Line & Delete Line functions in it, but I quickly discovered that it was useless, I did it just to see how to do it.

I since updated it with REFRESH buttons from the data source it's coming from, which is the "list_item_send_text_lines.txt", and most importantly a FILTER capacity. I added a class called Items that would initialize the list items when the application starts, so any filtering or refreshing will NOT keep on looking at the file. 

The filter function deletes & filters the list_view upon each key stroke. I can edit it to filter upon pressing enter, and it likely will be by the time you read this. There was a way to not need to delete the entire list_view called Link View Virtual Groups, but I could spend the extra time doing other things in other domains/areas. I love the filter functionality because it narrows down a list that was becoming too large for the text to copy.

And that concludes it.

!()[images/list_view_code.png]