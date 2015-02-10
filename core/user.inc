; Layout of the user area
; 
; used by the multitasker
.set USER_STATE, 0
.set USER_PAUSE, USER_STATE + 2

; excpection handling
.set USER_HANDLER, USER_PAUSE + 2

; numeric IO
.set USER_BASE, USER_HANDLER + 2

; character IO 
.set USER_EMIT, USER_BASE + 2
.set USER_KEY, USER_EMIT + 2

.set USER_G_IN, USER_KEY + 2
.set USER_REFILL, USER_G_IN + 2

.set SYSUSERSIZE, USER_REFILL + 2
; 
