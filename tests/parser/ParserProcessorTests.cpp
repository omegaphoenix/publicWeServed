#include "gtest/gtest.h"

#include "ParserProcessor.h"
#include "../../src/parser/ParserProcessor.h"
#include <cstdio>

static const char *test_config = "tests/parser/test_config";

/* --- Function Prototypes --- */
void print_map(strmap *string_map);

/** 
 * Test a simple situation 
 */
TEST(ParserProcessorTest, GetPort1)
{   
    // Parse a mocked input
    const std::string config_text = "port 8080;";
    std::stringstream config_stream(config_text);
    NginxConfigParser parser;
    NginxConfig config;
    parser.Parse(&config_stream, &config);

    // Make sure that the port number is as expected
    ParserProcessor parser_processor = ParserProcessor(config);
    unsigned short port = parser_processor.get_port();
    EXPECT_EQ(8080, port);
}

/** 
 * Test a situation where the port is in a different line in a file 
 */
TEST(ParserProcessorTest, GetPort2)
{   
    // Parse a mocked input
    const std::string config_text = "port \n \n 8080;";
    std::stringstream config_stream(config_text);
    NginxConfigParser parser;
    NginxConfig config;
    parser.Parse(&config_stream, &config);
    
    // Make sure that the port number is as expected
    ParserProcessor parser_processor = ParserProcessor(config);
    unsigned short port = parser_processor.get_port();
    EXPECT_EQ(8080, port);
}

/** 
 * Test a situation where the port is within a block in a file 
 * with other items in it. 
 */
TEST(ParserProcessorTest, GetPortFile)
{	
    // Parse the input test file
	NginxConfigParser config_parser;
    NginxConfig config;
    config_parser.Parse(test_config, &config);
    
    // Make sure that the port number is as expected
    ParserProcessor parser_processor = ParserProcessor(config);
    unsigned short port = parser_processor.get_port();
    EXPECT_EQ(8080, port);
}

/**
 * Test a simple situation
 */
TEST(ParserProcessorTest, GetPath1)
{
    // Parse a mocked input
    const std::string config_text("static1 /files;");
    std::stringstream config_stream(config_text);
    NginxConfigParser parser;
    NginxConfig config;
    parser.Parse(&config_stream, &config);

    // Make sure that the paths are as expected
    ParserProcessor parser_processor = ParserProcessor(config);
    strmap *paths = parser_processor.get_paths();

    // No paths found, fail test
    if (paths == NULL)
        FAIL() << "No paths found by get_paths()";

    strmap expected_values({{"static1", "/files"}});
    EXPECT_EQ(*paths, expected_values);
}

/**
 * Test a simple situation
 */
TEST(ParserProcessorTest, GetPath2)
{
    // Parse a mocked input
    const std::string config_text("static1 /videos;\n\n"
                                  "static2 /images;\n\n");
    std::stringstream config_stream(config_text);
    NginxConfigParser parser;
    NginxConfig config;
    parser.Parse(&config_stream, &config);

    // Make sure that the paths are as expected
    ParserProcessor parser_processor = ParserProcessor(config);
    strmap *paths = parser_processor.get_paths();

    // No paths found, fail test
    if (paths == NULL)
        FAIL() << "No paths found by get_paths()";

    strmap expected_values({{"static1", "/videos"},
                            {"static2", "/images"}});
    EXPECT_EQ(*paths, expected_values);
}

/**
 * Test a situation where the paths are within a block in a file
 * with other items in it.
 */
TEST(ParserProcessorTest, GetPathsFile)
{
    // Parse the input test file
    NginxConfigParser config_parser;
    NginxConfig config;
    config_parser.Parse(test_config, &config);

    // Make sure that the paths are as expected
    ParserProcessor parser_processor = ParserProcessor(config);
    strmap *paths = parser_processor.get_paths();

    // No paths found, fail test
    if (paths == NULL)
        FAIL() << "No paths found by get_paths()";

    strmap expected_values({{"static1", "/images"},
                            {"static2", "/videos"},
                            {"static3", "/htmls"}});
    EXPECT_EQ(*paths, expected_values);
}


/**
 * Debugging function: prints the given string map
 */
void print_map(strmap *string_map)
{
    for ( unsigned i = 0; i < string_map->bucket_count(); ++i) {
        std::cout << "bucket #" << i << " contains:";
        for ( auto local_it = string_map->begin(i); local_it!= string_map->end(i); ++local_it )
            std::cout << " " << local_it->first << ":" << local_it->second;
        std::cout << std::endl;
    }
}




/* 
 * Test for ProxyHandler
 */

class RequestHandlerTest : public ::testing::Test {
    // Added 5/7/2015
    // Assignment 5 unit tests

    // Modified 5/24/2015 to work with new handlers.
    protected:

    bool HandleProxyResponse(const HTTPRequest &req, 
            const std::string &host, const std::string &port, const std::string &expected) {
        ProxyHandler a;
        std::string output;
        size_t check_http;
        size_t check_domain;
        size_t check_content;
        size_t check_date;

        NginxConfig config;
        std::shared_ptr<NginxConfigStatement> statement (new NginxConfigStatement);
        statement->tokens_.push_back("host");
        statement->tokens_.push_back(host);
        config.statements_.push_back(port);
        a.Configure(config);

        output = a.HandleRequest(req);


        check_http = output.find("HTTP/1.0 200 OK\r\n");
        check_domain = output.find(expected);
        check_content_type = output.find("Content-Type: text/html;\r\n");     
        check_date = output.find("GMT");

        if (check_http == std::string::npos ||
            check_content_type == std::string::npos ||
            check_date == std::string::npos ||
            check_domain == std::string::npos) {
            return 0;
        }
        return 1;     
    }  

};

// Request Handler Tests:
TEST_F(RequestHandlerTest, HandleProxyResponse) {
    // Tests the google.com proxy from port 80.
    HTTPRequest test_static1;
    test_static1.method = "GET";
    test_static1.path = "/reverse"; 
    EXPECT_TRUE(HandleProxyResponse(test_static1, "www.google.com", "80", "domain=.google.com"));


}
