GLOBAL getSystemYear
GLOBAL getSystemMonth
GLOBAL getSystemDayOfMonth
GLOBAL getSystemHour
GLOBAL getSystemMin
GLOBAL getSystemSec
getSystemYear:
	mov al, 9 
	out 70h, al 
	in al, 71h 
	ret
getSystemMonth:
	mov al, 8 
	out 70h, al 
	in al, 71h 
	ret
getSystemDayOfMonth:
	mov al, 7 
	out 70h, al 
	in al, 71h 
	ret
getSystemHour:;+3
	mov al, 4
	out 70h, al 
	in al, 71h 
	ret
getSystemMin:
	mov al, 2 
	out 70h, al 
	in al, 71h 
	ret
getSystemSec:
	mov al, 0 
	out 70h, al 
	in al, 71h 
	ret