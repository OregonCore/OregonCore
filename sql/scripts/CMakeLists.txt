########### install files ###############

FILE(GLOB _SQL *.sql)
FILE(GLOB README readme.txt)

install(FILES ${_SQL} DESTINATION share/oregon/sql/scripts)
install(FILES ${README} DESTINATION share/oregon/sql/scripts)
