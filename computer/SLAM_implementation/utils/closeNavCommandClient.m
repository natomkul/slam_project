function client = closeNavCommandClient(client)
% closeNavCommandClient  Close nav TCP client; returns [] for caller assignment.

    if isempty(client)
        return;
    end

    delete(client);
    client = [];
end
