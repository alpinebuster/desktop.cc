# desktop-cpp

A dental assisted treatment platform that supports intelligent detection of marker points and fully automated head measurement.

## Dir description

|  Name  | Description  |
|  ----  | ----  |
|[_installer](./_installer/README.md)|Release directory|
|[_release](./_release/README.md)|Package directory|
|[contrib](./contrib/README.rst)|Personal record|
|[milab](./milab/README.md)|Project home directory|
|[server](./server/README.md)|The back-end code|
|[thirdparty](./thirdparty/README.md)|Third-party module|

## Associate issue when you commit code

* To commit the code, just include the issue number, #issue_id, in the comment:
    `git commit -m "#issue_id First commit"`
* Automatically close issue when you finished your code:
    `git commit -m "close #issue_id I finished my code"` or
    `git commit -m "fix #issue_id I fixed my code"`
* Merge on GitHub website

## Qt and OS

* Tested with Qt 5.15.2 and Qt6
* Tested on Windows 7, Windows 10, MacOSX 11.0.1

## Installation

### Qt online installer & Build Tools for Visual Studio 2019

### Part Ⅰ

* Download Qt online installer
* Sign up for a QT account
* Log in and choose to install Qt 5.15.2
* Select Components:
  * Click on the drop-down menu in the Qt bar and select Qt 5.15.2
  * Select WebAssembly, MSVC 2019 64-Bit, Sources, Qt WebEngine, Qt WebGL Streaming Plugin
  * Click Developer and Designer Tools in the bottom column to open the drop-down menu and select Qt Installer Framework 4.1
* Wait for the software to download and install

### Part Ⅱ

* Visit [Visual Studio download page](https://visualstudio.microsoft.com/downloads/) in the browser, in the All Downloads section of the page，select Tools for Visual Studio 2019 drop-down menu
* Download Build Tools for Visual Studio 2019（Visual Studio Installer）
* Once the Visual Studio Installer has downloaded and installed, start it and select Desktop Development with C++
* After the installation is complete, a reboot may be required to take effect.

Note: the network may not load properly, please use proxy.

## Coding style

* [Indentation](https://wiki.qt.io/Qt_Coding_Style#Indentation)
* [Declaring variables](https://wiki.qt.io/Qt_Coding_Style#Declaring_variables)
* [Whitespace](https://wiki.qt.io/Qt_Coding_Style#Whitespace)
* [Braces](https://wiki.qt.io/Qt_Coding_Style#Braces)
* [Parentheses](https://wiki.qt.io/Qt_Coding_Style#Parentheses)
* [Switch statements](https://wiki.qt.io/Qt_Coding_Style#Switch_statements)
* [Jump statements (break, continue, return, and goto)](https://wiki.qt.io/Qt_Coding_Style#Jump_statements_.28break.2C_continue.2C_return.2C_and_goto.29)
* [Line breaks](https://wiki.qt.io/Qt_Coding_Style#Line_breaks)
* [General exceptions](https://wiki.qt.io/Qt_Coding_Style#General_exceptions)
* [Artistic Style](https://wiki.qt.io/Qt_Coding_Style#Artistic_Style)
* [clang-format](https://wiki.qt.io/Qt_Coding_Style#clang-format)

### Pass the file name

* The API needs to use file names in portable format, i.e., use slashes (/) instead of backslashes (\) even on Windows. To pass a user-entered filename to the API, first convert it using QDir::fromNativeSeparators. To display the filename to the user, use QDir::toNativeSeparators to convert it back to native format. For these tasks, consider using Utils::FileName::fromUserInput(QString) and Utils::FileName::toUserOutput()

### Naming rules

* Prefix MILab to different module names
* Different files within the same module name with the module prefix
* Header file:

    ```cpp
    public: // method
        explicit MainWindow(QWidget *parent = Q_NULLPTR);
        virtual ~MainWindow();
        void setContent(QWidget *w);
    public: // variable
        
    private: // method
        bool leftBorderHit(const QPoint &pos);
        bool rightBorderHit(const QPoint &pos);
        bool topBorderHit(const QPoint &pos);
        bool bottomBorderHit(const QPoint &pos);
        void styleWindow(bool bActive, bool bNoState);
        void syncPosition();
    private: // variable
        Ui::MainWindow *ui;
        QRect m_StartGeometry;
        const quint8 CONST_DRAG_BORDER_SIZE = 15;
        bool m_bMousePressed;
        bool m_bDragTop;
        bool m_bDragLeft;
        bool m_bDragRight;
        bool m_bDragBottom;
    ```

### Doxygen

Doxygen can convert specific annotations in a codebase into a documentation file. It can generate a pure reference manual by extracting the structure of the code or visualizing the relationships between documents with the help of automatically generated dependency graphs, inheritance diagrams, and collaboration diagrams to visualize the relationships between documents. Doxygen generates help files in formats such as CHM, RTF, PostScript, PDF, HTML, etc.

* [Download Doxygen](https://www.doxygen.nl/download.html)
* This chapter [Special Commands](https://www.doxygen.nl/manual/commands.html) covers two topics:
  * How to put comments in your code such that doxygen incorporates them in the documentation it generates.
  * Ways to structure the contents of a comment block such that the output looks good.
* Common usage:
    | Command | Description |
    |  ----  | ----  |
    | `\file` | Endorsement notes for files |
    | `\author` | Author's information |
    | `\brief` | Suggested description for a class or function, e.g. `\brief This function is responsible for printing error messages` |
    | `\param` | Mainly used in function descriptions, followed by the name of the parameter, then followed by a description of the parameter |
    | `\return` | Describe the return value of the function, e.g. this function returns the result of the execution, true if it succeeds, otherwise it returns false |
    | `\retval` | Description of the return value type |
    | `\note` | - |
    | `\attention` | - |
    | `\warning` | - |
    | `\enum` | Referencing an enum, `doxygen` will generate a link at that enum, e.g. `\enum CTest::CEnum` |
    | `\var` | When a variable is referenced, `doxygen` will generate a link at that enumeration |
    | `\class` | Reference to a class, format: `\class [][]` |
    | `\exception` | Description of possible exceptions, e.g. `\exception The execution of this function may generate an out-of-scope exception` |

### Graphviz

Graphviz (Graph Visualization Software) is used to draw graphs described by DOT language scripts. To use Doxygen to generate dependency graphs, inheritance graphs, and collaboration graphs, you must first install the graphviz software.

* [Download Graphviz](https://graphviz.org/download/) for Windows
* `brew install graphviz` for Mac

## Attention

* The compiler's issue column will prompt *Expected Token ","*, just ignore it.
* Must add .qmake.conf to top source directory

    ```txt
    Qt 5 makes the problem a non-challenge.

    Create a .qmake.conf file in your top-level source dir which contains these two lines:

    top_srcdir=$$PWD top_builddir=$$shadowed($$PWD)

    Done.
    ```

* Using chinese when compile with MSVC compiler is **not** recommended.
* The compiler may throw an "*.lib" not found error when you try run the project for the first time, just run it again.
