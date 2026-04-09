import os, json

from helper import SyzlangHelper

if __name__ == "__main__":
    print(os.getcwd())
    with open('./chat_history_stream_order/syzlang.json', 'r') as f:
        data = f.read()
        syzlang = SyzlangHelper()
        j = json.loads(data)
        print(syzlang.validate_syzlang_json(j))
        print(syzlang.generate_description(j))
        print("")
        print("# CONST")
        print(syzlang.generate_const(j))