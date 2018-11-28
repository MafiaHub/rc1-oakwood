#NoEnv  ; Recommended for performance and compatibility with future AutoHotkey releases.
#Warn  ; Enable warnings to assist with detecting common errors.
SendMode Input  ; Recommended for new scripts due to its superior speed and reliability.

RootFolder := RegExReplace(A_ScriptDir, "\\[^\\]+$")
BinaryFolder := RootFolder "\Bin\Debug"

SetWorkingDir, %BinaryFolder%  ; Ensures a consistent starting directory.

;
; First instance
;
Run oakwood.exe

; console
WinWait Oakwood: dev-console
WinActivate
WinSetTitle, oakwood-cli-1
WinMove, oakwood-cli-1,, 0, (A_ScreenHeight / 1.5), (A_ScreenWidth / 2), (A_ScreenHeight / 3)

; game window
WinWait LS3D engine
WinActivate
; WinSetTitle, oakwood-game-1
WinMove, LS3D engine,, 0, 0, (A_ScreenWidth / 2), (A_ScreenHeight / 1.5)

;
; Second instance
;
Run oakwood.exe

; console
WinWait Oakwood: dev-console
WinActivate
WinSetTitle, oakwood-cli-2
WinMove, oakwood-cli-2,, (A_ScreenWidth / 2), (A_ScreenHeight / 1.5), (A_ScreenWidth / 2), (A_ScreenHeight / 3)

; game window
WinWait LS3D engine
WinActivate
; WinSetTitle, oakwood-game-2
WinMove, LS3D engine,, (A_ScreenWidth / 2), 0, (A_ScreenWidth / 2), (A_ScreenHeight / 1.5)
