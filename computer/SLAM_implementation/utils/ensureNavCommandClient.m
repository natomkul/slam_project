function client = ensureNavCommandClient(client, host, port)
% ensureNavCommandClient  Try once to open nav TCP; no wait, no warning if down.

    if ~isempty(client)
        return;
    end

    if nargin < 2 || strlength(string(host)) == 0
        host = "127.0.0.1";
    end
    if nargin < 3 || isempty(port)
        port = 5006;
    end

    try
        client = tcpclient(host, port);
        fprintf("Nav command channel connected (%s:%d).\n", host, port);
    catch
        client = [];
    end
end
