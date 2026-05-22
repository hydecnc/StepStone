from argparse import ArgumentParser
from cons import *
from commands import Command

class GenerateCommand(Command):
    def __init__(self):
        super().__init__()
        self.args = None
    
    def custom_subparser(self, parser: ArgumentParser, cmd: str):
        return parser.add_parser(cmd, help='Generate syzlang with LLM')
    
    def add_arguments(self, parser: ArgumentParser):
        super().add_arguments(parser)

        parser.add_argument('--setup', action='store_true',
                            help='Enable console mode')
        parser.add_argument('--output', nargs='?', action='store',
                            help='Using time stamp as output folder name if not specified')
        parser.add_argument('--config', nargs='?', action='store',
                            help='config file. Will be overwritten by arguments if conflict.')
        parser.add_argument('--llm', choices=['chatgpt'],
                            help='chatgpt')
        parser.add_argument('--engine', choices=['gpt-4-turbo', 'gpt-4o', 'o1-preview', 'o1-mini', 'o1'],
                            help='')
        parser.add_argument('--api', nargs='?', action='store',
                            help='send a single api')
        parser.add_argument('--api-file', nargs='?', action='store',
                            help='')
        parser.add_argument('--join-syzlang-json', nargs='?', action='store',
                            help='')
        parser.add_argument('--refer-syzlang-context', nargs='?', action='store',
                            help='')
        parser.add_argument('--chat-type', nargs='?', action='store',
                            help='assistant | conversation')
        parser.add_argument('--verbose', action='store_true',
                            help='verbose mode')
        
    def run(self, args):
        self.args = args
        if self.args.setup:
            cons = CusConsole(mode=MODE_INTERACTIVE, debug=self.args.verbose, args=self.args)
            cons.display()
        else:
            cons = CusConsole(mode=MODE_CLI, args=self.args, debug=self.args.verbose)
            if self.args.api_file != None:
                if not os.path.exists(self.args.api_file):
                    raise FileExistsError(self.args.api_file)
                f = open(self.args.api_file, 'r')
                cuda_api_text = f.readlines()
            elif self.args.api != None:
                cuda_api_text = [self.args.api]
            else:
                return
            cons.cli_run(cuda_api_text)
