# Level 0:
add_subdirectory(coreplugin)

# Level 1: (only depends of Level 0)
add_subdirectory(texteditor)
add_subdirectory(helloworld)
add_subdirectory(imageviewer)
add_subdirectory(marketplace)
add_subdirectory(updateinfo)
add_subdirectory(welcome)

# Level 2: (only depends on Level 1 and below)
add_subdirectory(projectexplorer)

# Level 3: (only depends on Level 2 and below)
add_subdirectory(bookmarks)
add_subdirectory(help)

# Level 4: (only depends on Level 3 and below)
add_subdirectory(todo)
