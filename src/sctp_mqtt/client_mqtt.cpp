#include "base_sctp.cpp"

class ClientMQTT : BaseSCTP
{
protected:
public:
    ClientMQTT()
    {
        service = 7733;
        af_family = AF_INET;

        if (prepare_client() == 1)
        {
            fprintf(stderr, "prepare_client error!\n");
            exit(1);
        }

        if (set_options() == 1)
        {
            fprintf(stderr, "set_options error!\n");
            exit(1);
        }
    }
};
