#!/usr/bin/python3

from optparse import OptionParser
import sys
import os

def parse_args(args):
    parser = OptionParser()

    parser.add_option("--root", default=".", help="Root path to create the class in")
    parser.add_option("--filename", help="Path to the file, relative to root")

    return parser.parse_args(args)

def pathSplitAll(path):
    head, tail = os.path.split(path)
    print("head: {0}, tail: {1}".format(head, tail))
    if head:
        head = pathSplitAll(head)
        if tail:
            head.append(tail)
        print(head)
        return head

    else:
        if tail:
            return [tail]
        else:
            return []
    

def makeIncludeGuard(path_components):
   pathcomponent = "__".join([x.upper() for x in path_components])
   include_guard = "__{0}_H__".format(pathcomponent)
   return include_guard

def makeInclude(path_components):
    include = '#include "{0}"'.format(makeHeaderFilename(path_components))
    return include

def makeBaseFilename(path_components):
    return "/".join(path_components)

def makeHeaderFilename(path_components):
    return "{0}.h".format(makeBaseFilename(path_components))

def makeCppFilename(path_components):
    return "{0}.cpp".format(makeBaseFilename(path_components))

def makeNamespaces(path_components):
    start = []
    end = []
    for elem in path_components:
        start.append("namespace {0} {{".format(elem))
        end.append("}}  // namespace {0}".format(elem))

    end.reverse()

    start = "\n".join(start)
    end = "\n".join(end)

    return (start, end)

def makeClassDef(class_name):
    return """/*
 * \\brief Class {0}
 */    
class {0} {{
  public:

  private:
}};""".format(class_name)

def main(args):
    options, args = parse_args(args)

    # remove comon prefix and sanitize
    commonPath = os.path.commonpath((options.filename, options.root))
    filename = options.filename[len(commonPath):]
    filename = filename.strip(os.sep)
    filename = filename.strip('/')

    # Split all path components
    path_components = pathSplitAll(filename)
    filename = path_components[-1]
    path_components = path_components[:-1]

    # remove file extension
    class_name, _ = os.path.splitext(filename)

    # TODO: make class_name be CamelCase

    print("Class name: {0}.Path: {1}".format(class_name, path_components))

    namespaces_start, namespaces_end = makeNamespaces(path_components)
    classDef = makeClassDef(class_name)

    path_components.append(class_name)

    include_guard = makeIncludeGuard(path_components)
    include = makeInclude(path_components)

 

    print("Include Guard: {0}".format(include_guard))

    print("Include: {0}".format(include))

    header_filename = os.path.join(options.root, makeHeaderFilename(path_components))
    cpp_filename = os.path.join(options.root, makeCppFilename(path_components))

    print("Header: {0}, CPP: {1}".format(header_filename, cpp_filename))
    
    if (not os.path.exists(header_filename) and not os.path.exists(cpp_filename)):
        print("Creating files")
        with open(header_filename, 'w') as headerfile:
            headerfile.write("""#ifndef {0}\n#define {0}\n\n{1}\n\n{3}\n\n{2}\n\n#endif  // {0}\n""".format(include_guard, namespaces_start, namespaces_end, classDef))
        
        with open(cpp_filename, 'w') as cppfile:
            cppfile.write("""{0}\n\n{1}\n\n{2}\n""".format(include, namespaces_start, namespaces_end))


    else:
        print("At least one of the files already exists")


    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))