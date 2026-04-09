import sys, os
sys.path.append(os.getcwd())

import argparse
from structparser import code_parser

def parse_args():
    parser = argparse.ArgumentParser(description='Ftrace Parser')
    parser.add_argument('--build-db', nargs='?', type=str, help='path of db folder')
    parser.add_argument('--source-file', nargs='?', type=str, help='path of source file')
    parser.add_argument('--find', nargs='?', type=str, help='find a symbol')
    parser.add_argument('--select-db', nargs='?', type=str, help='Specify a database for lookup')
    parser.add_argument('--expand', '-e', action='store_true', help='iterate data structure in fields')
    parser.add_argument('--ignore', nargs='?', type=str, help='Ignore a list of symbols, separated by comma')
    parser.add_argument('--list-obj', nargs='?', type=str, help='List objects, do not show their definitions')
    return parser.parse_args()

def build_db(file_path, db_path):
    codep.build_db(file_path, db_path)

if __name__ == '__main__':
    args = parse_args()
    codep = code_parser.CodeParser()
    
    if args.build_db != None or args.source_file != None:
        if args.build_db == None:
            print("--build-db is required for building database")
            exit(0)
        if args.source_file == None:
            print("--source-file is required for building database")
            exit(0)
        build_db(args.source_file, args.build_db)
        exit(0)
        
    if args.find != None:
        if args.select_db == None:
            print("Use --select-db to specify a database")
            exit(0)
        codep.init_db(args.select_db)
        codep.find(args.find, iterate=args.expand, ignore=args.ignore)
    
    if args.list_obj != None:
        if args.select_db == None:
            print("Use --select-db to specify a database")
            exit(0)
        codep.init_db(args.select_db)
        print(codep.list_objects(args.list_obj, ignore=args.ignore))