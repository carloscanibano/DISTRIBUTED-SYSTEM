program topic_server {
	version topic_server_v1 {
		int init()  = 1;
		int store_topic_text(string topic_name, string topic_text) = 2;
		string retrieve_topic_text(string topic_name) = 3;
	} = 1;
} = 99;