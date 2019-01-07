extern "C" {
#include "sc_memory_headers.h"
#include "utils.h"
#include "sc_helper.h"
}

#include <stdio.h>
#include <iostream>

#define NUMBER_PRECISE 8

using namespace std;

void printEl(sc_memory_context *context, sc_addr element)
{
    sc_addr idtf;
    sc_type type;
    sc_memory_get_element_type(context, element, &type);

    if ((sc_type_node & type) == sc_type_node)
    {

        if (SC_RESULT_OK == sc_helper_get_system_identifier_link(context, element, &idtf))
        {
            sc_stream *stream;
            sc_uint32 length = 0, read_length = 0;
            sc_char *data;
            sc_memory_get_link_content(context, idtf, &stream);
            sc_stream_get_length(stream, &length);
            data = (sc_char *)calloc(length + 1, sizeof(sc_char));
            sc_stream_read_data(stream, data, length, &read_length);
            data[length] = '\0';
            printf("%s", data);
            sc_stream_free(stream);
            free(data);
        }
        else
        {
            printf("%u|%u", element.seg, element.offset);
        }
    }
    else
    {
        sc_addr elem1, elem2;
        sc_memory_get_arc_begin(context, element, &elem1);
        sc_memory_get_arc_end(context, element, &elem2);
        printf("(");
        printEl(context, elem1);
        printf(" -> ");
        printEl(context, elem2);
        printf(")");
    }
}

void printContent(sc_memory_context *context, sc_addr element)
{
    sc_stream *stream;
    sc_uint32 length = 0, read_length = 0;
    sc_char *data;
    if (sc_memory_get_link_content(context, element, &stream) != SC_RESULT_OK)
    {
        printf("Content reading error\n");
    }
    sc_stream_get_length(stream, &length);
    data = (sc_char *)calloc(length + 1, sizeof(sc_char));
    sc_stream_read_data(stream, data, length, &read_length);
    data[length] = '\0';
    printf("%s", data);
    sc_stream_free(stream);
    free(data);
}

int getInt(sc_memory_context *context, sc_addr element)
{
    int res = 0;
    sc_stream *stream;
    sc_uint32 length = 0, read_length = 0;
    sc_char *data;
    if (SC_RESULT_OK != sc_memory_get_link_content(context, element, &stream))
    {
        printf("Can't read content\n");
        return -1;
    }
    sc_stream_get_length(stream, &length);
    data = (sc_char *)calloc(length + 1, sizeof(sc_char));
    sc_stream_read_data(stream, data, length, &read_length);
    data[length] = '\0';

    res = atoi(data);

    sc_stream_free(stream);
    free(data);
    return res;
}

int getFloat(sc_memory_context *context, sc_addr element)
{
    int res = 0;
    sc_stream *stream;
    sc_uint32 length = 0, read_length = 0;
    sc_char *data;
    if (SC_RESULT_OK != sc_memory_get_link_content(context, element, &stream))
    {
        printf("Can't read content\n");
        return -1;
    }
    sc_stream_get_length(stream, &length);
    data = (sc_char *)calloc(length + 1, sizeof(sc_char));
    sc_stream_read_data(stream, data, length, &read_length);
    data[length] = '\0';

    res = atof(data);

    sc_stream_free(stream);
    free(data);
    return res;
}

sc_addr genIntNode(sc_memory_context *context, sc_uint32 data)
{
    sc_stream *stream;
    sc_addr link;
    char *content = (char *)calloc(NUMBER_PRECISE, sizeof(sc_char));
    snprintf(content, NUMBER_PRECISE, "%d", data);

    link = sc_memory_link_new(context);
    stream = sc_stream_memory_new(content, strlen(content), SC_STREAM_FLAG_READ, SC_FALSE);
    if (SC_RESULT_OK != sc_memory_set_link_content(context, link, stream))
    {
        free(content);
        sc_memory_element_free(context, link);
        SC_ADDR_MAKE_EMPTY(link);
        sc_stream_free(stream);
        return link;
    }
    free(content);
    sc_stream_free(stream);
    return link;
}

sc_addr genFloatNode(sc_memory_context *context, float data)
{
    sc_stream *stream;
    sc_addr link;
    char *content = (char *)calloc(NUMBER_PRECISE, sizeof(sc_char));
    snprintf(content, NUMBER_PRECISE, "%f", data);

    link = sc_memory_link_new(context);
    stream = sc_stream_memory_new(content, strlen(content), SC_STREAM_FLAG_READ, SC_FALSE);
    if (SC_RESULT_OK != sc_memory_set_link_content(context, link, stream))
    {
        free(content);
        sc_memory_element_free(context, link);
        SC_ADDR_MAKE_EMPTY(link);
        sc_stream_free(stream);
        return link;
    }
    free(content);
    sc_stream_free(stream);
    return link;
}

sc_addr getNode(sc_memory_context *context, const char *sys_idtf)
{
    sc_addr n;
    sc_helper_resolve_system_identifier(context, sys_idtf, &n);
    return n;
}

sc_addr findRrel(sc_memory_context *context, sc_addr rrel_0, const char *rrel_sys_idtf)
{
    sc_addr rrel_node, node;
    node = sc_memory_node_new(context, sc_type_const);
    sc_helper_resolve_system_identifier(context, rrel_sys_idtf, &rrel_node);
    sc_iterator5 *it = sc_iterator5_f_a_a_a_f_new(context,
                                                  rrel_0,
                                                  sc_type_arc_pos_const_perm,
                                                  0,
                                                  sc_type_arc_pos_const_perm,
                                                  rrel_node);
    if (SC_TRUE == sc_iterator5_next(it))
    {
        node = sc_iterator5_value(it, 2);
    }
    else
    {
        printf("Not Exist\n");
    }
    sc_iterator5_free(it);
    return node;
}
