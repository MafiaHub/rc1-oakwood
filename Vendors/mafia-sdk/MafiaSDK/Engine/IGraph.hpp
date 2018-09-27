/*
	Copyright 2017 Dávid Svitana

	Licensed under the Apache License, Version 2.0 (the "License");
	you may not use this file except in compliance with the License.
	You may obtain a copy of the License at

	http://www.apache.org/licenses/LICENSE-2.0

	Unless required by applicable law or agreed to in writing, software
	distributed under the License is distributed on an "AS IS" BASIS,
	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
	See the License for the specific language governing permissions and
	limitations under the License.
*/

#ifndef _IGraph_H_
#define _IGraph_H_

namespace MafiaSDK
{
    constexpr unsigned long MAIN_WINDOW = 0x101C5458;
	constexpr unsigned long CHILD_WINDOW = 0x101C5408;

    HWND GetMainWindow() 
    {
        return *(HWND*)(MAIN_WINDOW);
    }

	HWND GetChildWindow() 
    {
        return *(HWND*)(CHILD_WINDOW);
    }

    bool IsWindowFocused() 
    {
        return GetMainWindow() == GetActiveWindow();
    }
}

#endif