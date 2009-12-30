########### install files ###############

FILE(GLOB _SQL *.sql)
FILE(GLOB README readme.txt)

install(FILES ${_SQL} DESTINATION share/oregon/sql/full_tables)
install(FILES ${README} DESTINATION share/oregon/sql/full_tables)
