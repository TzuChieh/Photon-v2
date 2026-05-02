import contextlib
import http.server
import os
import socket
import socketserver
import webbrowser
from pathlib import Path


class ReportServer(socketserver.TCPServer):
    allow_reuse_address = True


class ReportHandler(http.server.SimpleHTTPRequestHandler):
    def do_POST(self):
        if self.path == "/__shutdown":
            self.send_response(204)
            self.end_headers()
            self.server.shutdown_requested = True
            return
        self.send_response(404)
        self.end_headers()


def _find_free_port() -> int:
    with contextlib.closing(socket.socket(socket.AF_INET, socket.SOCK_STREAM)) as sock:
        sock.bind(("127.0.0.1", 0))
        sock.listen(1)
        return int(sock.getsockname()[1])


def main():
    report_dir = Path.cwd() / "RenderTest" / "report_output"
    report_file = report_dir / "report.html"

    if not report_dir.exists():
        raise FileNotFoundError(f"report output directory not found: {report_dir}")
    if not report_file.exists():
        raise FileNotFoundError(f"report file not found: {report_file}")

    port = _find_free_port()
    handler = ReportHandler

    os.chdir(report_dir)

    with ReportServer(("127.0.0.1", port), handler) as httpd:
        httpd.shutdown_requested = False
        url = f"http://127.0.0.1:{port}/report.html"
        print(f"Serving RenderTest report at {url}")
        print("Server stops on shutdown signal from page close or Ctrl+C.")
        print("Press Ctrl+C to stop.")
        webbrowser.open(url)
        try:
            while not httpd.shutdown_requested:
                httpd.handle_request()
        except KeyboardInterrupt:
            print("\nStopped.")
        finally:
            print("Server stopped.")


if __name__ == "__main__":
    main()
