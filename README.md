CS220 Project Framework
===============

## Project Group

Bryan Marsh

Jason Ott

## Usage

Usage: ./grid_router [<test_file> <algorithm> <bi-directional> <intersection> <korn modifier>]
The defaults are as follows:
 - test_file: ../Tests/debug.json
 - algorithm: lee
 - bi-directional: false
 - intersection: false
 - korn modifier: 1.0
 
### Argument space definition:

 - test_file: any json file that follows the documented format
 - algorithm: lee | lee2bit | lee3bit | ruben | hadlock
 - bi-directional: 1 | 0 (translates to true or false)
 - intersection: 1 | 0 (translates to true or false)
 - korn modifier: >= 1.0
 
## Assumptions

As we are operating within a framework provided, we will state some assumptions in an effort to remove any ambiguity

 **Assumption 1:**
    We assume that for intersections you can share a valid path.  This means that paths can share a some, none, or most of a given path of a previous run
    
 **Assumption 2:**
    A valid path is one where a source successfully connects to it's given sink as defined in the problem space given the arguments provided for program use.  If the defined source/sink cannot meet, the route is abandoned as a failure
    
 **Assumption 3:**
    While we check for validity, we assume all input will be in the form of 0 to n-1.  Input that does not meet this assumption will not result in an error, and that route or problem object will be invalid
    
 **Assumption 4:**
        To continue on Assumption 3, we assume that if the region of blockage size + it's starting x/y coordinate exceeds that of the size of the map space, we abandon solving the problem.  We will assume that there are other problems in the problem definition file.
    
 **Assumption 5:**
    We do not accept one source routing to multiple sinks.  To put it simple, we will only route a problem that has a unique one-to-one relationship.
    
 **Assumption 6:**
    If a source and sink have the same coordinates, the route is deemed un-routable.
    
## Project Overview

You will use this framework for the following projects:

- Grid Routing
- Advanced Grid Routing
- Soukup Global Grid Routing
- Line Routing

The project will involve reading in JSON test files (located in the test directory) and parsing these files into a Problem Object. This step has already been done for you and is written at the beginning of main.cc to allow for command line arguments, **You should not change this code** as it will be used for testing. Please note that the ProblemObject gives you the width and height of the chip, which will be indexed from 0 to n-1.

You will then take this Problem Object, and pass it into your custom problem map object. The custom problem map object is the object you use to reprsent the problem space. In the example we give that object is the Grid object, however **you should not use the grid object** as it is not adequate for this project. You may, however, want to take advantage of the other classes we give you, such as the Node and Edge classes. While it is not required that you use any classes we give you (except for the ProblemObject and Path/Netlist) it is highly encouraged. You will also find that many of the classes we provide are too general for your use, in this case we advise you to extend the base class with a child class designed to fit your needs.

Once you have created your custom problem map object, you should make a call to a routing algorithm (the specific algorithm will depend on which submission you are working on). You should strive to be able to use the same custom problem object for mulitple routers, however it is not required that all routes work with the same custom problem object. Your routing algorithm **must return either a Path/vector of Paths or Netlist/vector of Netlists** depending on what the situation calls for.

- Path: A path is a set of straight line segments from one source to one sink
- Netlist: A netlist is a set of straight line segments from one souce to multiple sinks

Once you have a solution in the form of a Path/vector of Paths or Netlist/vector of Netlists, you should print your solution to std out for review.

Currently there is only one test case given, and it is not a good test case. It is given as an example so you can create your own test cases. **You should be creating robust test cases** that are capable of testing malformed input files, edge cases, and problems that are impossible to solve. With this **your program should error gracefully**, meaning if there is a bad input file or some type of error the program should not simply crash. It should give an explination as to the possible error and exit. We have given you the claim system for printing errors, please use it.

**Please Note:** If you have one of the projects listed at the top, then your project **MUST** exist within this framework.

## Running the Framework

In order to run the framework, you must first go to Utilities/JSON_parser and run "make". This will create a json_parse.so file that is need to parse the JSON test files that we give you. Next you will go back to the root directory and run "make". This will generate an executable called "grid_router", which you can run by calling "./grid_router <test_case>", where <test_case> is the path of a test case. In the framework we give you there is only one test case, located at "Tests/test_sample.json". This will run the example that we have written for the framework.

**Please Note:** some of you may get a library error when trying to run grid_router. If you recieve this library error, please copy the file "json_parser.so" from Utilities/JSON_parser to the root directory where the file "grid_router" exists and this should fix the problem. Note that you must have already called "make" in Utilities/JSON_parser for "json_parse.so" to exist.

## Bugs

As I have stated before, we expect there to be bugs in this framework. When you find a bug please [write a good bug report](http://www.chiark.greenend.org.uk/~sgtatham/bugs.html) and submit it to the GitHub bug page. Or better yet create a fix to the problem and [submit a pull request](https://help.github.com/articles/using-pull-requests/). You should also be **checking the bug tracking page** of the repository for known problems before emailing me or submitting a new bug report.

## Your Repository

For your submission, you must have a repository updated with the code you wish to submit by 8:00AM PST on the due date. Your repository must also adhere to the following rules.

- Your repository must be private
- I must be [added as a collaborator on your repository](https://help.github.com/articles/adding-collaborators-to-a-personal-repository/) (username: brrcrites)
- Your README.md must at minimum have the name(s) of your group members (and hopefully more useful information)

In addition to this, you **must email me with a link to your repsitory**. There are currently many repositories existing, and I have no idea who most of them belong to. **In order to have a private repository you must have a student developer pack**. You [get the student developer pack here](https://education.github.com/pack), and it comes with alot of other cool stuff. If you do not have the pack, and therefore cannot make your repository private, before the deadline for your proejct, do not worry. Continue to update your repository and make it private as soon as you can.

