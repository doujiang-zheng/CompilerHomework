//
//  fa.cpp
//  CompilerHomework
//
//  Created by 郑铜亚 on 16/5/10.
//  Copyright © 2016年 郑铜亚. All rights reserved.
//

#include "fa.hpp"
#include <utility>
#include <map>
#include <vector>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <cstdio>
#include <cstring>
using namespace std;

fa::fa() {}

int fa::init(char file[])
{
    //freopen(file, "r", stdin);
    ifstream input(file);
    string temp;
    
    while((input >> temp) && temp != "###")
    {// As defined, Non terminal symbols should be written firstly with an end of "#".
        this->vn.push_back(temp);
    }
    this->vn.push_back(FINAL);
    this->init_graph();
    
    while((input >> temp) && temp != "###")
    {
        this->vt.push_back(temp);
    }
    
    string temp_a, temp_b, temp_c;
    while(input >> temp_a >> temp_b >> temp_c)
    {// An example : S -> 0A, written as S 0 A , a : S,  b : 0, c : A
        // a production as : A -> EPSILON, written as A EPSILON FINAL
        //                   A -> 1, written as A 1 FINAL
        auto cur_p = find(this->vn.begin(), this->vn.end(), temp_a);
        auto next_p = find(this->vn.begin(), this->vn.end(), temp_c);
        // list_node* node =  new list_Node();
        // would come to compiler's optimization.
        // node always use the same memory.
        list_node* node = new list_node();
        node->edge = temp_b, node->next_id = next_p - this->vn.begin();
        this->nfa_graph.at(cur_p - this->vn.begin()).push_back(*node);
    }
    return 0;
}

int fa::init_graph()
{
    for(int i = 0; i < this->vn.size(); i ++)
    {
        nfa_node* nt = new nfa_node();
        nt->id = i;
        this->nfa_graph.push_back(*nt);
    }
    return 0;
}

int fa::nfa_print()
{
    cout << endl << "VN:";
    for(auto i = this->vn.begin(); i != this->vn.end(); i ++)
        cout << *i << ' ';
    cout << endl << "VT:";
    for(auto i = this->vt.begin(); i != this->vt.end(); i ++)
        cout << *i << ' ';
    cout << endl << "Grah: ";
    for(auto i = this->nfa_graph.begin(); i != this->nfa_graph.end(); i ++)
    {
        cout << endl << i - this->nfa_graph.begin() << ": ";
        for(auto j = i->out_edge.begin(); j != i->out_edge.end(); j ++)
            cout << j->edge << " -> " << j->next_id << " | ";
    }
    cout << endl;
    return 0;
}

vector<dfa_node> fa::to_dfa()
{
    vector<long> first; first.push_back(0);
    move_epsilon(first);
    this->dfa_vn.push_back(first);
    dfa_node* node = new dfa_node();
    node->state_id = 0, node->flag = UNFLAG;
    this->dfa_graph.push_back(*node);
    
    for(int i = 0; i < this->dfa_vn.size(); i ++)
    {
        if (this->dfa_graph.at(i).flag != UNFLAG)
            continue;
        if (find(this->dfa_vn.at(i).begin(), this->dfa_vn.at(i).end(), this->vn.size() - 1) != this->dfa_vn.at(i).end())
            this->dfa_final.insert(i);
        for(int j = 0; j < this->vt.size(); j ++)
        {
            vector<long> ret; ret.clear();
            move_specific(this->dfa_vn.at(i), this->vt.at(j), ret);
            if (ret.size() != 0)
            {
                move_epsilon(ret);
                //solve_epsilon(ret, fin);
                list_node* node = new list_node();
                node->edge = this->vt.at(j);
                auto next_id = find(this->dfa_vn.begin(), this->dfa_vn.end(), ret);
                if (next_id == this->dfa_vn.end())
                {
                    // ATTETION ! vector may change location in the Memory when be pushed back;
                    this->dfa_vn.push_back(ret);
                    node->next_id = this->dfa_vn.size() - 1;
                    // A new status_set produced;
                    dfa_node* d_node = new dfa_node();
                    d_node->state_id = this->dfa_vn.size() - 1, d_node->flag = UNFLAG;
                    this->dfa_graph.push_back(*d_node);
                }
                else
                    node->next_id = next_id - this->dfa_vn.begin();
                this->dfa_graph.at(i).push_back(*node);
            }
            
        }
        
    }
    this->dfa_graph.at(0).flag = START;
    return this->dfa_graph;
}

bool fa::move_epsilon(vector<long>& p)
{
    for (int i = 0; i < p.size(); i ++)
    {
        for (auto j = this->nfa_graph.at(p.at(i)).out_edge.begin();
             j != this->nfa_graph.at(p.at(i)).out_edge.end();
             j ++)
        {
            if (j->edge == EPSILON && find(p.begin(), p.end(), j->next_id) == p.end())
            {
                p.push_back(j->next_id);
                //printf("solve_epsilon: %ld\n", j->next_id);
            }
        }
    }
    sort(p.begin(), p.end());
    return true;
}

bool fa::move_specific(vector<long> p, string e, vector<long>& ret)
{
    for (int i = 0; i < p.size(); i ++)
    {
        //printf("Current Id: %ld \n", *i);
        for (auto j = this->nfa_graph.at(p.at(i)).out_edge.begin();
             j != this->nfa_graph.at(p.at(i)).out_edge.end();
             j ++)
        {
            if (j->edge == e && find(ret.begin(), ret.end(), j->next_id) == ret.end())
            {
                ret.push_back(j->next_id);
//                printf("solve_move: %s -> %ld |", j->edge.c_str(),j->next_id);
            }
        }
    }
    //printf("\n");
    // Sort the set before return, in this case the algorithm of find() will do correct search.
    sort(ret.begin(), ret.end());
    return true;
}

int fa::minimize_dfa()
{
    
    return 0;
}

int fa::dfa_print()
{
    printf("\nDFAGraph:");
    for(auto i = this->dfa_graph.begin(); i != this->dfa_graph.end(); i ++)
    {
        printf("\n%ld :", i - this->dfa_graph.begin());
        for(auto k = this->dfa_vn.at(i - this->dfa_graph.begin()).begin();
            k != this->dfa_vn.at(i - this->dfa_graph.begin()).end();
            k ++)
            printf("%ld ", *k);
        printf("\n");
        for (auto j = i->out_edge.begin(); j != i->out_edge.end(); j ++)
        {
            printf(" %s -> %ld |", j->edge.c_str(), j->next_id);
        }
    }
    cout << "\nFINAL STATE :\n";
    for(auto i = this->dfa_final.begin(); i != this->dfa_final.end(); i ++)
        cout << *i << ' ';
    cout << endl;
    return 0;
}

int fa::dfa_match(string cmp)
{
    int max = 0; long state = 0;
    if(this->dfa_final.find(0) != this->dfa_final.end())
        max = 0;    // A - > (a | b)*
    for(int i = 0; i < cmp.length(); i ++)
    {
        dfa_node tmp = this->dfa_graph.at(state);
        char c = cmp.at(i);
        bool next = false;
        for(int j = 0; j < tmp.out_edge.size(); j ++)
        {
            if(tmp.out_edge.at(j).edge == string(1, c))
            {
                next = true;
                state = tmp.out_edge.at(j).next_id;
                if(this->dfa_final.find(state) != this->dfa_final.end())
                    max = i + 1;
                break;
            }
        }
        if(!next)
            break;
    }
    
    return max;
}

int fa::lr_init(char file[])
{
    //freopen(file, "r", stdin);
    ifstream input(file);
    string temp;
    
    this->lr_vn.push_back("S'");    // LR(1) analysis need an extended 2 degree grammar.
    while(input >> temp && temp != "###")
    {
        this->lr_vn.push_back(temp);
    }
    this->lr_vt.push_back("###");
    while(input >> temp && temp != "###")
    {
        this->lr_vt.push_back(temp);
    }
    
    this->lr_production.resize(this->lr_vn.size());
    vector<string> start; start.push_back(this->lr_vn.at(1)); // The first VN is seen as the start symbol.
    this->lr_production.at(0).push_back(start); // LR(1) analysis need an extended 2 degree grammar.
    while(input >> temp)
    {
        long index = find(this->lr_vn.begin(), this->lr_vn.end(), temp) - this->lr_vn.begin();
        if (index == this->lr_vn.size())
            cerr << temp << ": Undefined VN in LR productions.\n";
        vector<string> v;
        string tmpa;
        while(input >> tmpa && tmpa != "###")
        {
            long index_a = find(this->lr_vn.begin(), this->lr_vn.end(), tmpa) - this->lr_vn.begin();
            if(index_a == this->lr_vn.size() && tmpa != EPSILON &&
               find(this->lr_vt.begin(), this->lr_vt.end(), tmpa) == this->lr_vt.end())
                printf("%s: Undefined VN or VT in LR productions.\n", tmpa.c_str());
            v.push_back(tmpa);
        }
        
        lr_production.at(index).push_back(v);
    }
    
    /*for(int i = 0; i < this->lr_vn.size(); i ++)
    {
        cout << this->lr_vn.at(i) << " : " << endl;
        for(int j = 0; j < this->lr_production.at(i).size(); j ++)
        {
            cout << " -> ";
            for(int k = 0; k < this->lr_production.at(i).at(j).size(); k ++)
                cout << this->lr_production.at(i).at(j).at(k);
            cout << endl;
        }
    }*/
    solve_first();
    build_lr_graph();
    return 0;
}

int fa::solve_epsilon()
{
#ifndef UNDEFINED
#define UNDEFINED -1
#define YES 1
#define NO 0
#endif
    vector<int> lr_vn_flag(this->lr_vn.size(), UNDEFINED);
    vector<vector<int>> lr_production_flag(this->lr_vn.size());
    
    for(int i = 0; i < this->lr_vn.size(); i ++)
    {   // First scanning, ensure the directive epsilon VN && non-epsilon VN.
        // When ending, all productions containging VTs or VNs with a NO-EPSILON flag are filtered.
        auto res = this->lr_production.at(i);
        // After resizeing, vector variable may change location in the Memory.
        // To avoid iterator becoming wild, get the refrence after vector resizing.
        // The same as VN flags, UNDEFINED means that the production is filled with undefined VN.
        //                       YES means the right component of the productions contains only a EPSILON
        //                        NO means the right component of the production contains VTs or NO-EPSILON VNs.
        lr_production_flag.at(i).resize(this->lr_production.at(i).size(), UNDEFINED);
        
        int no_pro_count = 0; // Count the specific VN's NO-EPSILON productions.
        for(int j = 0; j < res.size(); j ++)
        {
            auto pro = res.at(j);
            if (pro.size() == 1 && *pro.begin() == EPSILON)
            {   // Search for the EPSILON right component.
                lr_vn_flag.at(i) = YES;
                lr_production_flag.at(i).at(j) = YES;
                break;
            }
            for(int k = 0; k < pro.size(); k ++)
            {
                if (find(this->lr_vt.begin(), this->lr_vt.end(), pro.at(k)) != this->lr_vt.end())
                {   // A production containing a VT should be flaged NO
                    no_pro_count ++;
                    lr_production_flag.at(i).at(j) = NO; break;
                    //flag.at(j) = NO; break;
                }
            }
        }
        if (no_pro_count == lr_production_flag.at(i).size())
            // If all productions are flaged NO, then flag the VN with NO.
            lr_vn_flag.at(i) = NO;
    }
    
    bool expand_flag = true;
    while (expand_flag)
    {   // Next, scanning the productions of all VNs cycle by cycle,
        // until not any new VNs are added into the epsilon set.
        expand_flag = false;
        for(int i = 0; i < this->lr_vn.size(); i ++)
        {
            if (lr_vn_flag.at(i) != UNDEFINED)
                continue;
            auto res = this->lr_production.at(i);
            auto flag = lr_production_flag.at(i);
            int no_pro_count = 0;
            for(int j = 0; j < res.size(); j ++)
            {
                if (lr_vn_flag.at(i) != UNDEFINED)
                    // When changing the state during scanning the productions of the specific VN,
                    // break.
                    break;
                
                if(flag.at(j) == NO)
                {
                    no_pro_count ++; break;
                }
                auto pro  = res.at(j);
                for(int k = 0; k < pro.size(); k ++)
                {
                    long index = find(this->lr_vn.begin(), this->lr_vn.end(), pro.at(k)) - this->lr_vn.begin();
                    if (lr_vn_flag.at(index) == YES && k + 1 == pro.size())
                    {   // Scaaning the last VN of the production.
                        lr_vn_flag.at(i) = YES;
                        lr_production_flag.at(i).at(j) = YES;
                        expand_flag = true; break;
                    }
                    if (lr_vn_flag.at(index) == YES)
                        // Skip the EPSILON VN.
                        continue;
                    if (lr_vn_flag.at(index) == NO)
                    {
                        no_pro_count++; flag.at(j) = NO; break;
                    }
                }
            }
            if (no_pro_count == flag.size())
            {
                lr_vn_flag.at(i) = NO;
                expand_flag = true;
            }
        }
    }
    
    for(int i = 0; i < lr_vn_flag.size(); i ++)
        if (lr_vn_flag.at(i) == YES)
            this->lr_epsilon_vn.insert(i);
    /*for(int i = 0; i < lr_vn_flag.size(); i ++)
    {
        if (lr_vn_flag.at(i) == YES)
            printf("%s YES\n", this->lr_vn.at(i).c_str());
        else if(lr_vn_flag.at(i) == NO)
            printf("%s NO\n", this->lr_vn.at(i).c_str());
        else
            printf("%s UNDEFINED\n", this->lr_vn.at(i).c_str());
    }
    for(int i = 0; i < lr_production_flag.size(); i ++)
    {
        printf("%d :", i);
        for(int j = 0; j < lr_production_flag.at(i).size(); j ++)
        {
            if (lr_production_flag.at(i).at(j) == YES)
                printf(" YES |");
            else if (lr_production_flag.at(i).at(j) == NO)
                printf(" NO |");
            else
                printf(" UNDEFINED |");
        }
        printf("\n");
    }*/
    return 0;
}

int fa::solve_first()
{
    this->solve_epsilon();
    /*for (int i = 0; i < this->lr_epsilon_vn.size(); i ++)
    {
        printf("%d : %s\n", i, this->lr_vn.at(this->lr_epsilon_vn.at(i)).c_str());
    }*/
    
    this->lr_first.resize(this->lr_vn.size());
    bool expand_flag = true;
    while(expand_flag)
    {
        expand_flag = false;
        for(int i = 0; i < this->lr_vn.size(); i ++)
        {
            long first_size = lr_first.at(i).size();
            auto res = this->lr_production.at(i);
            for(int j = 0; j < res.size(); j ++)
            {
                for(int k = 0; k < res.at(j).size(); k ++)
                {
                    auto index = find(this->lr_vn.begin(), this->lr_vn.end(), res.at(j).at(k)) - this->lr_vn.begin();
                    if (index == this->lr_vn.size())
                    {   // Come to a VT.
                        lr_first.at(i).insert(res.at(j).at(k));
                        break;
                    }
                    else
                    {   // Come to a VN.
                        lr_first.at(i).insert(lr_first.at(index).begin(), lr_first.at(index).end());
                        if (this->lr_epsilon_vn.find(index) == this->lr_epsilon_vn.end())
                            break;  // Break out if this VN cannot inudct EPSILON.
                    }
                    
                }
            }
            if(!expand_flag && first_size < lr_first.at(i).size())
                expand_flag = true; // If once expanded, turn the flag to true.
        }
    }
    
//    FILE* out = fopen("first.out", "w");
//    fprintf(out, "hello, world\n");
    for(int i = 0; i < this->lr_first.size(); i ++)
    {
        printf("%d %s :", i, this->lr_vn.at(i).c_str());
//        fprintf(out, "%d %s :", i, this->lr_vn.at(i).c_str());
//         Save EPSILON VNs only in the lr_epsilon_vn.
        this->lr_first.at(i).erase(EPSILON);
        for(auto j = this->lr_first.at(i).begin(); j != this->lr_first.at(i).end(); j ++)
            printf(" %s ", (*j).c_str());
//            fprintf(out, " %s ", (*j).c_str());
        printf("\n");
    }
    return 0;
}

int fa::build_lr_graph()
{
    lr_dfa_node* node = new lr_dfa_node();
    // Extended grammar, S' -> •S, S' is the first and start VN
    node->vn_id = 0, node->production_id = 0, node->pointer = 0, node->first_s.insert("###");
    vector<lr_dfa_node> start;
    start.push_back(*node);
    set<lr_dfa_node> start_ret;
    this->solve_closure(start, start_ret);
    lr_dfa_vn.push_back(start);

    for(int i = 0; i < lr_dfa_vn.size(); i ++)
    {   // Just like transforming NFA to DFA, try each VN and VT to build_lr_graph.
        vector<lr_dfa_node> cur = lr_dfa_vn.at(i);
        dfa_node d_node; d_node.state_id = i;
        
        // Solve VN move
        for(int j = 0; j < lr_vn.size(); j ++)
        {
            vector<lr_dfa_node> tmp;
            set<lr_dfa_node> ret;
            for(int k = 0; k < cur.size(); k ++)
            {
                lr_dfa_node k_node = cur.at(k);
                vector<string> k_production = lr_production.at(k_node.vn_id).at(k_node.production_id);
                if(k_node.pointer != k_production.size() &&
                   k_production.at(k_node.pointer) == lr_vn.at(j))
                { // Move the right VN and the pointer
                    lr_dfa_node n_node;
                    n_node.vn_id = k_node.vn_id;
                    n_node.production_id = k_node.production_id;
                    n_node.pointer = k_node.pointer + 1;
                    n_node.first_s.insert(k_node.first_s.begin(), k_node.first_s.end());
                    tmp.push_back(n_node);
                }
            }
            solve_closure(tmp, ret);
            if(tmp.size() > 0)
            {
                long index = find(lr_dfa_vn.begin(), lr_dfa_vn.end(), tmp) - lr_dfa_vn.begin();
                if(index == lr_dfa_vn.size())
                    lr_dfa_vn.push_back(tmp);
                list_node l_node; l_node.edge = lr_vn.at(j); l_node.next_id = index;
                d_node.push_back(l_node);
            }
        }
        
        // Solve VT move
        for(int j = 0; j < lr_vt.size(); j ++)
        {
            vector<lr_dfa_node> tmp;
            set<lr_dfa_node> ret;
            for(int k = 0; k < cur.size(); k ++)
            {
                lr_dfa_node k_node = cur.at(k);
                vector<string> k_production = lr_production.at(k_node.vn_id).at(k_node.production_id);
                if(k_node.pointer != k_production.size() &&
                   k_production.at(k_node.pointer) == lr_vt.at(j))
                { // Move the right VT and the pointer
                    lr_dfa_node n_node;
                    n_node.vn_id = k_node.vn_id;
                    n_node.production_id = k_node.production_id;
                    n_node.pointer = k_node.pointer + 1;
                    n_node.first_s.insert(k_node.first_s.begin(), k_node.first_s.end());
                    tmp.push_back(n_node);
                }
            }
            solve_closure(tmp, ret);
            if(tmp.size() > 0)
            {
                long index = find(lr_dfa_vn.begin(), lr_dfa_vn.end(), tmp) - lr_dfa_vn.begin();
                if(index == lr_dfa_vn.size())
                    lr_dfa_vn.push_back(tmp);
                list_node l_node; l_node.edge = lr_vt.at(j); l_node.next_id = index;
                d_node.push_back(l_node);
            }
        }
        lr_dfa_graph.push_back(d_node);
    }
    
    return 0;
}

int fa::solve_closure(vector<lr_dfa_node>& tmp, set<lr_dfa_node>& ret)
{
    bool expand_flag = true;
    while(expand_flag)
    {
        expand_flag = false;
        long pre_size = tmp.size();
        for(int i = 0; i < tmp.size(); i ++)
        {
            lr_dfa_node node = tmp.at(i);
            vector<string> cur_production = lr_production.at(node.vn_id).at(node.production_id);
            
            if(node.pointer < cur_production.size() &&
               find(lr_vn.begin(), lr_vn.end(), cur_production.at(node.pointer)) != lr_vn.end())
            {   // The pointer is not in the end && the next symbol is a VN, then extend its productions.
                long vn_index = find(lr_vn.begin(), lr_vn.end(), cur_production.at(node.pointer)) - lr_vn.begin();
                set<string> first_search;     // Solve the first_search set firstly.

                for(long i = node.pointer + 1; i <= cur_production.size(); i ++)
                {
                    if(i == cur_production.size())
                        first_search.insert(node.first_s.begin(), node.first_s.end());
                    else
                    {
                        long next_index  = find(this->lr_vn.begin(), this->lr_vn.end(), cur_production.at(i))
                        - this->lr_vn.begin();
                        if(next_index == this->lr_vn.size())
                        {    // Next symbol is a VT, break out the circle of solving first search set.
                            first_search.insert(cur_production.at(i));
                            break;
                        }
                        else
                        {
                            first_search.insert(lr_first.at(next_index).begin(), lr_first.at(next_index).end());
                            if (lr_epsilon_vn.find(next_index) == lr_epsilon_vn.end())
                                // This VN cannot induct EPSILON, break out the circle of solving first search set.
                                break;
                        }
                    }
                }
                
                for(int i = 0; i < lr_production.at(vn_index).size(); i ++)
                {
                    lr_dfa_node* new_node =  new lr_dfa_node();
                    new_node->vn_id = vn_index, new_node->production_id = i, new_node->pointer = 0;
                    new_node->first_s.insert(first_search.begin(), first_search.end());
                    ret.insert(*new_node);
                }
            }

            // Use a data structure of set to merge older tmp with extended lr_dfa_nodes
            // and erase duplicate lr_dfa_nodes.
            set<lr_dfa_node> s(tmp.begin(), tmp.end());
            s.insert(ret.begin(), ret.end());
            tmp.assign(s.begin(), s.end());
            // Reduce or Move into
        }
        if(pre_size < tmp.size())
            expand_flag = true;
    }
    
    set<lr_dfa_node> duplicate;
    for(auto i = tmp.begin(); i != tmp.end(); )
    {// set<template T> cannot be modified when built, only can be replaced or erased.
    // So I choose vector to merge duplicat nodes: S -> •Aa, # S -> •Aa, b
        auto pre = i ++;
        lr_dfa_node* node = new lr_dfa_node();
        node->vn_id = pre->vn_id, node->production_id = pre->production_id;
        node->pointer = pre->pointer, node->first_s.insert(pre->first_s.begin(), pre->first_s.end());
        
        while(i != tmp.end() &&
              pre->vn_id == i->vn_id &&
              pre->production_id == i->production_id &&
              pre->pointer == i->pointer)
        {
            node->first_s.insert(i->first_s.begin(), i->first_s.end());
            i ++;
        }
        duplicate.insert(*node);
    }
    
    tmp.assign(duplicate.begin(), duplicate.end());
    return 0;
}

int fa::lr_dfa_print()
{
    FILE* out = fopen("lr_dfa.out", "w");
    for(int i = 0; i < lr_dfa_vn.size(); i ++)
    {
        fprintf(out, "I%d :\n", i);
        for(int j = 0; j < lr_dfa_vn.at(i).size(); j ++)
        {
            lr_dfa_node node = lr_dfa_vn.at(i).at(j);
            fprintf(out, "%s -> ", lr_vn.at(node.vn_id).c_str());
            vector<string> production = lr_production.at(node.vn_id).at(node.production_id);
            for(int k = 0; k < production.size(); k ++)
            {
                if(k == node.pointer)
                    fprintf(out, "•");
                fprintf(out, "%s", production.at(k).c_str());
            }
            if(node.pointer == production.size())
                fprintf(out, "•");
            fprintf(out, " ,");
            for(auto k = node.first_s.begin(); k != node.first_s.end(); k ++)
                fprintf(out, "%s", (*k).c_str());
            fprintf(out, "\n");
        }
    }
    return 0;
}

int fa::lr_graph_print()
{
    FILE* out = fopen("graph.out", "w");
    for(int i = 0; i < lr_dfa_graph.size(); i ++)
    {
        dfa_node node = lr_dfa_graph.at(i);
        fprintf(out, "I%ld => \n", node.state_id);
        for(int j = 0; j < node.out_edge.size(); j ++)
        {
            list_node l_node = node.out_edge.at(j);
            fprintf(out, "I%ld -%s-> I%ld\n", node.state_id, l_node.edge.c_str(), l_node.next_id);
        }
    }
    fprintf(out, "Status");
    for(int i = 0; i < lr_vt.size(); i ++)
        fprintf(out, "%5s", lr_vt.at(i).c_str());
    for(int i = 0; i < lr_vn.size(); i ++)
        fprintf(out, "%5s", lr_vn.at(i).c_str());
    printf("\n");
    for(int i = 0; i < lr_dfa_vn.size(); i ++)
    {
        fprintf(out, "%5d", i);
        vector<pair<int, long>> v = lr_action_table.at(i);
        for(int j = 0; j < v.size(); j ++)
        {
            char c = get<0>(v.at(j)) == ACC ? 'A' :
                    (get<0>(v.at(j)) == REDUCE ? 'R':
                            (get<0>(v.at(j)) == MOVE ? 'S' : ' '));
                fprintf(out, "%4c%ld", c,get<1>(v.at(j)));
        }
        vector<long> vae = lr_goto_table.at(i);
        for(int j = 0; j < vae.size(); j ++)
            fprintf(out, "%5ld", vae.at(j));
        fprintf(out, "\n");
    }
    return 0;
}

int fa::build_lr_table()
{
    lr_action_table.resize(lr_dfa_vn.size());
    lr_goto_table.resize(lr_dfa_vn.size());
    for(int i = 0; i < lr_dfa_graph.size(); i ++)
    {   // Fill the move option.
        dfa_node node = lr_dfa_graph.at(i);
        vector<pair<int, long>> tmp_action(lr_vt.size(), make_pair(ERR, ERR)); // All empty cases are errors.
        vector<long> tmp_goto(lr_vn.size(), ERR);   // All empty cases are errors.
        for(int j = 0; j < node.out_edge.size(); j ++)
        {
            long index = find(lr_vn.begin(), lr_vn.end(), node.out_edge.at(j).edge) - lr_vn.begin();
            if(index < lr_vn.size())
                tmp_goto.at(index) = node.out_edge.at(j).next_id;
            else
            {
                index = find(lr_vt.begin(), lr_vt.end(), node.out_edge.at(j).edge) - lr_vt.begin();
                tmp_action.at(index) = make_pair(MOVE, node.out_edge.at(j).next_id);
            }
        }
        lr_action_table.at(i) = tmp_action;
        lr_goto_table.at(i) = tmp_goto;
    }
    
    for(int i = 0; i < lr_dfa_vn.size(); i ++)
    {   // Fill the reduce option.
        vector<lr_dfa_node> v = lr_dfa_vn.at(i);
        for(int j = 0; j < v.size(); j ++)
        {
            lr_dfa_node node = v.at(j);
            if(node.vn_id == 0 && node.production_id == 0 && node.pointer == 1)
            {
                lr_action_table.at(i).at(0) = make_pair(ACC, ACC);
            }
            else if(node.pointer == lr_production.at(node.vn_id).at(node.production_id).size() ||
                    (lr_production.at(node.vn_id).at(node.production_id)[0] == EPSILON))
            {
                for(auto k = node.first_s.begin(); k != node.first_s.end(); k ++)
                {
                    long index  = find(lr_vt.begin(), lr_vt.end(), *k) - lr_vt.begin();
                    if(get<0>(lr_action_table.at(i).at(index)) == MOVE)
                        printf("CONFLICT: State%d Production%d: MOVE IN && REDUCE\n", i, j);
                    if(get<0>(lr_action_table.at(i).at(index)) == REDUCE)
                        printf("CONFLICT: State%d Production%d: REDUCE && REDUCE\n", i, j);
                    lr_action_table.at(i).at(index) = make_pair(REDUCE, j); // Here store the i state's No.j production.
                }
            }
        }
    }
    return 0;
}
int fa::lr_stack_init()
{
    status_stack.push(0);
    symbol_stack.push("###");
    return 0;
}

int fa::lr_dfa_match(string cmp)
{
    printf("CURRENT STATE %ld: ", status_stack.top());
    long index  = find(this->lr_vt.begin(), this->lr_vt.end(), cmp) - this->lr_vt.begin();
    int ans = ERR;
    if(index >= this->lr_vt.size())
        return ans;
    long status = status_stack.top();
    int select = get<0>(lr_action_table.at(status).at(index));
    if(select == ERR)
        return ans;
    else if(select == MOVE)
    {
        status_stack.push(get<1>(lr_action_table.at(status).at(index)));
        symbol_stack.push(cmp);
        printf("MOVE IN %s GOTO %ld\n", cmp.c_str(), get<1>(lr_action_table.at(status).at(index)));
        ans = MOVE;
    }
    else if(select == REDUCE)
    {   // lr_production <=> lr_dfa_vn <=> lr_dfa_graph <=> lr_vn
        // Now  1. find the production_id in lr_dfa_vn status
        //      2. vector<string> means the right reduce production
        lr_dfa_node node = lr_dfa_vn[status][get<1>(lr_action_table[status][index])];
        vector<string> pro = lr_production.at(node.vn_id).at(node.production_id);
        printf("REDUCED AS %s => ", this->lr_vn.at(node.vn_id).c_str());
        for(int i = 0; i < pro.size() && pro[0] != EPSILON; i ++)
        {   // When coming to EPSILON, nothing in the stack should be poped.
            status_stack.pop(), symbol_stack.pop();
            printf("%s", pro.at(i).c_str());
        }
        printf("\n");
        this->lr_dfa_goto(lr_vn.at(node.vn_id));
        ans = REDUCE;
    }
    else    // ACC
        if(cmp == "###")
            ans = ACC;
    return ans;
}

int fa::lr_dfa_goto(string vn)
{
    long status = status_stack.top();
    long index = find(this->lr_vn.begin(), this->lr_vn.end(), vn) - this->lr_vn.begin();
//    if(index >= this->lr_vn.size())
//        return ERR;
    status_stack.push(lr_goto_table.at(status).at(index));
    symbol_stack.push(vn);
    return 0;
}