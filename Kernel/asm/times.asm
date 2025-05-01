GLOBAL get_system_year
GLOBAL get_system_month
GLOBAL get_system_day_of_month
GLOBAL get_system_hour
GLOBAL get_system_min
GLOBAL get_system_sec

get_system_year:
    mov al, 9 
    out 70h, al 
    in al, 71h 
    ret

get_system_month:
    mov al, 8 
    out 70h, al 
    in al, 71h 
    ret

get_system_day_of_month:
    mov al, 7 
    out 70h, al 
    in al, 71h 
    ret

get_system_hour:;+3
    mov al, 4
    out 70h, al 
    in al, 71h 
    ret

get_system_min:
    mov al, 2 
    out 70h, al 
    in al, 71h 
    ret

get_system_sec:
    mov al, 0 
    out 70h, al 
    in al, 71h 
    ret