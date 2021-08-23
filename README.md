[![logo](Docs/Logos/RegexAssitant_Logo.png)](https://github.com/David-Maisonave/RegexAssistant)
# RegexAssistant
[![All Releases](https://img.shields.io/github/downloads/David-Maisonave/RegexAssistant/total.svg)](https://github.com/David-Maisonave/RegexAssistant/releases/latest)

RegexAssistant is an open source Regular Expression (Regex) find and replace tool for Windows. 
It has an easy to use UI that can build and test regex, and the results are seen real time as expression is typed into the GUI.
It has a cheat-sheet token window which list the main regex tokens and common patterns. Items in the token list can be inserted into the Expression (regex) field by double clicking the desired token.

## Downloads
Use the following link to download the latest release binaries: [Download](https://github.com/David-Maisonave/RegexAssistant/releases/tag/v0.1.0.2)

## Features
[![Screenshot1](Docs/ProjectImages/RegexAssistantInAction.gif)](https://github.com/David-Maisonave/RegexAssistant)

### Real time results
The results are displayed in real time to quickly let the user see when an expression is working, and exactly when it stops working.

### Compatibility Option
The compatibility option allows the user to select the target regex implementation to display results for.
Currently available targets are Boost, Scintilla, POSIX, STD Regex, Perl, and SED.

### Regex Replace
Regex replace is supported when compatibility is switch to boost or STD types. The token list has example regex replacement patterns at the bottom of the list.
These patters can help the user to quickly test there replace expression, and user can easily undo the results by clicking the Undo button.

### Cheat-sheet (Token List)
A list window displays the regex tokens
The first column displays the token itself.  The second column gives a description.
The Third column gives example usage, and the last column displays matches for the examples.

When the user double clicks a token in the first column, the token gets inserted into the expression at the cursor position.

#### Cheat-sheet Common Patterns
The token window list many common patterns that the user can use just by double clicking on the desired patterns.
Here's a list of just a few of the many patterns that are listed.
##### Phone pattern
##### IP pattern
##### Date pattern
##### Time patterns
##### Zip code pattern
##### URL pattern
##### Major credit card pattern
##### File name pattern
##### Social Security Number pattern
##### Error reporting pattern
##### GUID pattern

### Convert FileSystem and SQL Wildcards to Regex
#### Convert FileSystem
This option is for users who are more familiar with windows file system wildcards (*?).
It allows the user to convert an expression using the file system wildcards into a regex expression, using regex wildcards.

#### Convert SQL
This option is for users who are more familiar with SQL wildcards (%_#*?).
It allows the user to convert an expression using the SQL wildcards into a regex expression, using regex wildcards.

# RegexAssistant Source Code
## Requirements

### Visual Studio 2017

 * *Community*, *Professional* or *Enterprise* Edition
 * VC++ 2017 v141 toolset
 * Windows XP support for C++
 * MFC and ATL support
 * Windows 10 SDK

### Visual Studio 2019

 * *Community*, *Professional* or *Enterprise* Edition
 * MSVC v142 Buildtools
 * C++ MFC for v142 Buildtools
 * Windows 10 SDK

## Folder Structure

Source base code for the RegexAssistant program is kept in the sub-folder RegexAssistant.
RegexAssistant requires two libraries (Boost and Scintilla).  These to libraries are not yet included in the project.

# Roadmap
1.	Add Replace support to Scintilla compatibility by using std::regex for replace.
2.	Add logic to allow window to shrink down further
3.	Add logic to remember last window size and position (to include monitor)
4.	Add split line to allow user to change size ratio of the token window against the sample window.
5.	Add button that will replace the target text with clipboard content
6.	Add button that will append the target text field with clipboard content
7.	Add command line option that will replace target text with clipboard content when the program starts
8.	Add command line option to populate the regex input field on program startup
9.	Add command line option to perform a regex replace on a file or multiple files matching DOS wildcards
10.	Add context menu item to replace (or append) sample with multiple sets of examples designed for each pattern listed in the token  list
11.	Add Build button which disables real time results
12.	Add a build wizard option that will let user go step by step creating an expression.
13.	Add logic to perform file find, rename, delete, & move based on the regex patterns
14.	Add drag and drop logic for sample window.

