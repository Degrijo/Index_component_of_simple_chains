extern "C" {
#include "sc_memory_headers.h"
#include "sc_helper.h"
#include "utils.h"
}
#include <cstring>
#include <math.h>
#include <stdio.h>
#include <iostream>
#include <glib.h>
#include <unistd.h>
#include <assert.h>
#include <vector>
#include <algorithm>

using namespace std;

sc_memory_context *context;

sc_addr graph, rrel_arcs, rrel_nodes;

int getWeight(sc_addr, sc_addr);

int toInt(sc_addr);

int g[10][10];

void mincut(char graph_name)
{
    vector <sc_addr> vec;
    char gr[3] = "Gx";
    gr[1] = graph_name;
    sc_helper_resolve_system_identifier(context, gr, &graph);
    sc_helper_resolve_system_identifier(context, "rrel_arcs", &rrel_arcs);
    sc_helper_resolve_system_identifier(context, "rrel_nodes", &rrel_nodes);
    sc_iterator5 *it=sc_iterator5_f_a_a_a_f_new(context, graph, sc_type_arc_pos_const_perm, 0, sc_type_arc_pos_const_perm, rrel_nodes);
    if (SC_TRUE == sc_iterator5_next(it)){
        sc_addr nodes = sc_iterator5_value(it, 2);
        sc_iterator3 *nodes_it = sc_iterator3_f_a_a_new(context, nodes, sc_type_arc_pos_const_perm, 0);
        while (SC_TRUE == sc_iterator3_next(nodes_it)){
            sc_addr t_node = sc_iterator3_value(nodes_it, 2);
            vec.push_back(t_node);
        }
        sc_iterator3_free(nodes_it);
    }
    sc_iterator5_free(it);
    sc_addr arcs;
    sc_iterator5 *it_arc=sc_iterator5_f_a_a_a_f_new(context, graph, sc_type_arc_pos_const_perm, 0, sc_type_arc_pos_const_perm, rrel_arcs);
    if (SC_TRUE == sc_iterator5_next(it_arc)){
         arcs = sc_iterator5_value(it_arc, 2);
    }
    sc_iterator5_free(it_arc);
    int size = vec.size();
    for (int l=0; l<size; ++l) g[l][0]=0;
    int k = 0;
    for (int i=size-1; i>=0; --i){
       sc_addr verti = vec[i];
       sc_iterator5 *vert = sc_iterator5_f_a_a_a_f_new(context, verti, sc_type_arc_common, 0, sc_type_arc_pos_const_perm, arcs);
       while (SC_TRUE == sc_iterator5_next(vert)){
           sc_addr t_vert = sc_iterator5_value(vert, 2);
           int c;
           for (int j=0; j<size; ++j){
               if (SC_ADDR_IS_EQUAL(vec[j], t_vert) == true) c=j;
               else g[j][k]=0;
           }
          g[c][k]=1;
          g[i][k]=1;
          k++;
       }
       sc_iterator5_free(vert);
    }
    for (int i=0;i<size; i++){
        for (int j =0; j<k; j++) cout << g[i][j]<< " ";
        cout << endl;
    }
    int razvetv = 0;
        vector <int> pozicii;
        for (int q = 0; q < size; q++) {
            for (int w = 0; w < k; w++) {
                if (g[q][w] == 1) {
                    razvetv++;
                    pozicii.push_back(w);
                }
            }
            while (razvetv > 2) {
                for (int y = 0; y < size; y++) if (g[y][pozicii[0]] == 1) g[y][pozicii[0]] = 0;
                pozicii.erase(pozicii.begin());
                razvetv--;
            }
            razvetv = 0;
        }
        double colvo = 0;
        int edenic = 0;
        for (int q = 0; q < size; q++) {
            for (int w = 0; w < k; w++) {
                if (g[q][w] == 1) edenic++;
            }
            if (edenic == 0) colvo += 1.0;
            if (edenic == 1) colvo += 0.5;
            edenic = 0;
        }
    cout << "Index component for the simple chain of the graph G" << graph_name << "=" << colvo << endl;
    cout << endl;
    vec.clear();
}

int toInt(sc_addr element)
{
    int number = -2;
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
            number = atoi(data);
            sc_stream_free(stream);
            free(data);
        }
    }
    return number;
}

int main()
{
    sc_memory_params params;

    sc_memory_params_clear(&params);
    params.repo_path = "/home/yaroslav/ostis/kb.bin";
    params.config_file = "/home/yaroslav/ostis/config/sc-web.ini";
    params.ext_path = "/home/yaroslav/ostis/sc-machine/bin/extensions";
    params.clear = SC_FALSE;

    sc_memory_initialize(&params);

    context = sc_memory_context_new(sc_access_lvl_make_max);
    cout << "////////////////////////////////////" << endl;
    cout << endl;
    mincut('0');
    mincut('1');
    mincut('2');
    mincut('3');
    mincut('4');
    cout << "////////////////////////////////////" << endl;
    sc_memory_context_free(context);

    sc_memory_shutdown(SC_TRUE);

    return 0;
}
