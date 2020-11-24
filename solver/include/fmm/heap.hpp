/**
 * min_heap class is needed to organize and store the TRIAL values
 * when running the solver.
 **/

#include <cmath>
#include <cstdint>
#include <iostream>
#include <vector>

#include "defs.hpp"

namespace fmm
{
    template<typename data_t>
    class min_heap
    {
      private:
        //! Node structure - value = node arrival time / index = map
        //! index
        struct node
        {
            //! Value and original map coordinates
            gridpoint_t node_data;
            //! Heap data vector index
            uint64_t index;
        };

        //! Heap nodes data
        std::vector<node> data;

        //! Updates the value of an existing node (NEEDS CHANGES)
        void update(gridpoint_t new_node, uint64_t index);

        uint64_t parent(uint64_t index) { return floor((index - 1) / 2); }

        uint64_t min_child(uint64_t index);

      public:
        //! Min_heap empty constructor
        min_heap() {}

        //! Min_heap constructor
        min_heap(uint64_t max_size);

        //! Returns the min node value
        data_t const min_node()
        {
            if(data.size() == 0)
                std::cerr << "Heap is empty." << std::endl;

            return data.node_data.value[0];
        }

        //! Inserts a node
        void insert_or_update(gridpoint_t new_node);

        //! Removes the node with smallest value
        void pop();

        //! Bubbles down a node
        void down_heap(uint64_t index);

        //! Bubbles up a node
        void up_heap(uint64_t index);

        //! Returns data vector size
        size_t size() { return data.size(); }

        //! Debug temp pruint64_t function
        void print();
    };
}    // namespace fmm

template<typename data_t>
fmm::min_heap<data_t>::min_heap(uint64_t max_size)
{
    data.reserve(max_size);
}

template<typename data_t>
uint64_t fmm::min_heap<data_t>::min_child(uint64_t index)
{
    if(data.size() <= 2 * index + 1)
    {
        return index;
    }
    else if(data.size() - 1 == 2 * index + 1)
    {
        return 2 * index + 1;
    }
    else
    {
        return (data.at(2 * index + 1).node_data.value <=
                data.at(2 * index + 2).node_data.value)
                   ? (2 * index + 1)
                   : (2 * index + 2);
    }
}

template<typename data_t>
void fmm::min_heap<data_t>::update(gridpoint_t new_node, uint64_t index)
{
    data.at(index).node_data = new_node;

    up_heap(index);
}

template<typename data_t>
void fmm::min_heap<data_t>::insert_or_update(gridpoint_t new_node)
{
    uint64_t node_index = 0;

    for(node_index = 0; node_index < data.size(); node_index++)
    {
        if(data.at(node_index).node_data.map_index == new_node.map_index)
        {
            if(new_node.value < data.at(node_index).node_data.value)
            {
                update(new_node, node_index);
            }
            return;
        }
    }

    data.push_back(node{ new_node, node_index });

    up_heap(data.size() - 1);
}

template<typename data_t>
void fmm::min_heap<data_t>::pop()
{
    if(data.size() == 0)
    {
        std::cerr << "Heap is empty." << std::endl;
        return;
    }

    std::swap(data.at(0), data.at(data.size() - 1));
    data.pop_back();

    down_heap(0);
}

template<typename data_t>
void fmm::min_heap<data_t>::down_heap(uint64_t index)
{
    for(uint64_t child = min_child(index);
        index < data.size() &&
        data.at(child).node_data.value < data.at(index).node_data.value;
        index = child, child = min_child(index))
    {
        std::swap(data.at(child), data.at(index));
    }
}

template<typename data_t>
void fmm::min_heap<data_t>::up_heap(uint64_t index)
{
    for(uint64_t par = parent(index);
        index > 0 &&
        data.at(index).node_data.value < data.at(par).node_data.value;
        index = par, par = parent(index))
    {
        std::swap(data.at(index), data.at(par));
    }
}

template<typename data_t>
void fmm::min_heap<data_t>::print()
{
    if(data.size() == 0)
    {
        std::cerr << "Heap is empty." << std::endl;
        return;
    }

    for(auto n = data.begin(); n != data.end(); ++n)
    {
        std::cout << "NODE " << n->index << " : VALUE = " << n->node_data.value
                  << ", MAP COORDINATES : (" << n->node_data.map_index.first
                  << "," << n->node_data.map_index.second << ")" << std::endl;
    }
    std::cout << "" << std::endl;
}