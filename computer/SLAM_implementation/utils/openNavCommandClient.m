function client = openNavCommandClient(host, port)
% openNavCommandClient  TCP client to C++ nav command server (move/rotate lines).
% Returns [] if the server is not listening yet.

    client = [];

    if nargin < 1 || strlength(string(host)) == 0
        host = "127.0.0.1";
    end
    if nargin < 2 || isempty(port)
        port = 5006;
    end

    try
        client = tcpclient(host, port);
    catch ME
        warning("Nav command TCP connect failed (%s:%d): %s", host, port, ME.message);
    end
end
