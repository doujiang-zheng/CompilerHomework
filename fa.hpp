//
//  fa.hpp
//  CompilerHomework
//
//  Created by 郑铜亚 on 16/5/10.
//  Copyright © 2016年 郑铜亚. All rights reserved.
//

#ifndef fa_hpp
#define fa_hpp

#include <stack>
#include <map>
#include <set>
#include <vector>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <cstdio>
#include <cstring>
using namespace std;

#define EPSILON "EPSILON"
#define FINAL "FINAL"

#define UNFLAG 0
#define FLAGED 1
#define START 2
#define END 3

#define ERR -1
#define ACC 0
#define REDUCE 1
#define MOVE 2

struct list_node
{
    // when turning nfa to dfa, need to set a flag
    // when minimize dfa,
    //long vis;
    string edge;
    long next_id;
    // Actually, whether nfa or dfa, node is a state
    // Only edge represents real string-pattern.
    // char next_string[MAXSIZE];
};

struct nfa_node
{
    //long vis;
    long id;
    vector<list_node> out_edge;
    int push_back(list_node node)
    {
        out_edge.push_back(node);
        return 0;
    }
};

struct dfa_node
{
    long flag;
    long state_id;
    vector<list_node> out_edge;
    int push_back(list_node node)
    {
        this->out_edge.push_back(node);
        return 0;
    }
};

struct lr_dfa_node
{
    long vn_id;
    long production_id;
    long pointer; // point to the right component S -> ~aAbBc
    set<string> first_s;
    
    bool operator<(const lr_dfa_node& r) const
    {
        if(vn_id < r.vn_id)
            return true;
        if(vn_id == r.vn_id && production_id < r.production_id)
            return true;
        if(vn_id == r.vn_id && production_id == r.production_id &&
           pointer < r.pointer)
            return true;
        if(vn_id == r.vn_id && production_id == r.production_id &&
           pointer == r.pointer && first_s < r.first_s)
            return true;
        return false;
    }
    
    bool operator==(const lr_dfa_node& r) const
    {
        return (vn_id == r.vn_id && production_id == r.production_id &&
                pointer == r.pointer && first_s == r.first_s);
        //return true;
    }
};

class fa
{
private:
//    set<string> vn;
//    set<string> vt;
    vector<string> vn;
    vector<string> vt;
    vector<nfa_node> nfa_graph;
    
    vector<vector<long>> dfa_vn;
    set<long> dfa_final;
    vector<dfa_node> dfa_graph;
    
//    set<string> lr_vn;
//    set<string> lr_vt;
    vector<string> lr_vn;
    vector<string> lr_vt;
    //multimap<long, vector<string>> lr_production;
    vector<vector<vector<string>>> lr_production;
    set<long> lr_epsilon_vn;
//    vector<long> lr_epsilon_vn;
    vector<set<string>> lr_first; // solve VNs' first set
    vector<vector<lr_dfa_node>> lr_dfa_vn; // each vector<lr_dfa_node> means a status in lr project sets
    vector<dfa_node> lr_dfa_graph; // Actually, to describe lr_dfa_graph is the same as common dfa_graph.
                                    // Compared to transfrom a NFA to DFA, flag is not needed.
    vector<vector<pair<int, long>>> lr_action_table; // <ACC, 0> | <REDUCE, 2> | <MOVE, 7>
    vector<vector<long>> lr_goto_table;
    
    stack<long> status_stack;
    stack<string> symbol_stack;
public:
    fa();
    int init(char file[]);
    int init_graph();
    int nfa_print();
    
    vector<dfa_node> to_dfa();
    bool move_epsilon(vector<long>& p);
    bool move_specific(vector<long> p, string e, vector<long>& ret);
    int minimize_dfa();
    int dfa_print();
    int dfa_match(string cmp);
    
    int lr_init(char file[]);
    int solve_epsilon();
    int solve_first();
    int solve_follow();
    int build_lr_graph();
    int solve_closure(vector<lr_dfa_node>& tmp, set<lr_dfa_node>& ret);
    int lr_dfa_print();
    int lr_graph_print();
    int build_lr_table();
    int lr_stack_init();
    int lr_dfa_match(string cmp);
    int lr_dfa_goto(string vn);
};

#endif /* fa_hpp */
