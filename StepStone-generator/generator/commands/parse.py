import json

from argparse import ArgumentParser
from cons import *
from commands import Command
from syzlang import SyzlangHelper

class ParseCommand(Command):
    def __init__(self):
        super().__init__()
        self.args = None
        self.syzlang_helper: SyzlangHelper = None
    
    def custom_subparser(self, parser: ArgumentParser, cmd: str):
        return parser.add_parser(cmd, help='Parse syzlang json')
    
    def add_arguments(self, parser: ArgumentParser):
        super().add_arguments(parser)

        parser.add_argument('--syzlang-json', '-j', nargs='?', action='store',
                            help='Path of the target syzlang json file')
        parser.add_argument('--syzlang-context', '-c', nargs='?', action='store',
                            help='Join a congregate syzlang json file')
        parser.add_argument('--output', nargs='?', action='store',
                            help='Save output to a folder.\n\
                            The generated description will be named as cuda.txt and cuda.txt.const')
        parser.add_argument('--verbose', action='store_true',
                            help='verbose mode')
        
    def run(self, args):
        self.args = args
        self.syzlang_helper = SyzlangHelper("NO HEADER")
        syzlang_data = None
        syzlang_congregated_data = None
        
        if self.args.syzlang_json == None:
            print("--syzlang-json cannot be empty.")
            return
        
        if not os.path.exists(self.args.syzlang_json):
            print("{} does not exist.".format(self.args.syzlang_json))
            return
        
        syzlang_data = json.load(open(self.args.syzlang_json, 'r'))
        if self.args.syzlang_context != None:
            if not os.path.exists(self.args.syzlang_context):
                print("{} does not exist".format(self.args.syzlang_context))
                return
            syzlang_congregated_data = json.load(open(self.args.syzlang_context, 'r'))
            
        base_path = os.getcwd()
        if self.args.output != None:
            base_path = self.args.output
        err = self.syzlang_helper.validate_syzlang_json(syzlang_data)
        if err != '':
            print("Validation failure: {}".format(err))
            return
        data = self.syzlang_helper.generate_description(syzlang_data, syzlang_congregated_data)
        data_path = os.path.join(base_path, 'cuda.txt')
        self.save_file(data, data_path)
        
        data = self.syzlang_helper.generate_const(syzlang_data)
        data_path = os.path.join(base_path, 'cuda.txt.const')
        self.save_file(data, data_path)
        
    def save_file(self, data, file_path):
        if os.path.exists(file_path):
            print("File path \"{}\" already exist".format(file_path))
            return
        with open(file_path, 'w') as fp:
            fp.write(data)