import socket
import time

def handle_client(conn, addr, llm_model):
    print(f"Connected by {addr}")
    try:
        while True:
            # 接收客户端输入
            data = conn.recv(4096)
            if not data:
                break
            text = data.decode().strip()
            if text.lower() == 'q':
                conn.sendall(b'Bye\n')
                break
            start = '0x8a8a8a8a'
            conn.sendall(start.encode('utf-8'))  # start
            # 调用 LLM 模型生成
            cont = llm_model.generate(text)

            full_cont = ''
            for c in cont:
                full_cont += c
                # 发送结果回客户端
                conn.sendall(c.encode('utf-8'))
            time.sleep(0.1)
            end = '0xa8a8a8a8'
            conn.sendall(end.encode('utf-8'))  # start
    finally:
        conn.close()
        print(f"Connection from {addr} closed.")

def main():
    from llm import LlmModel
    llm_model = LlmModel('deepseek-r1-1.5b')

    HOST = '127.0.0.1'
    PORT = 8888 

    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.bind((HOST, PORT))
        s.listen(1)
        print(f"Server listening on {HOST}:{PORT}")
        while True:
            conn, addr = s.accept()
            handle_client(conn, addr, llm_model)

if __name__ == '__main__':
    main()

