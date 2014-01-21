: .hex4 hex <# # # # # #> type space decimal ;
( addr1 cnt -- addr2)
: dmp
 over .hex4 [char] : emit space
 begin
   ?while 1- swap dup @i .hex4 1+ swap
 repeat
 drop
;

( start count -- )


