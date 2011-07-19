/*******************************************************************************
    Copyright (c) 2011, Yahoo! Inc.
    All rights reserved.

    Redistribution and use of this software in source and binary forms, 
    with or without modification, are permitted provided that the following 
    conditions are met:

    * Redistributions of source code must retain the above
      copyright notice, this list of conditions and the
      following disclaimer.

    * Redistributions in binary form must reproduce the above
      copyright notice, this list of conditions and the
      following disclaimer in the documentation and/or other
      materials provided with the distribution.

    * Neither the name of Yahoo! Inc. nor the names of its
      contributors may be used to endorse or promote products
      derived from this software without specific prior
      written permission of Yahoo! Inc.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS 
    IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED 
    TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A 
    PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
    OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
    SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
    LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, 
    DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY 
    THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
    OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

    The Initial Developer of the Original Code is Shravan Narayanamurthy.
******************************************************************************/
/*
 * TopKList.cpp
 *
 *  Created on: 14 May, 2009
 *      
 */

#include "TopKList.h"
#include <climits>
#include <iostream>

/**
 * Constructs a TopKList supporting
 * top K_ elements
 */
TopKList::TopKList(int K_) {
    K = K_;
    num_elements = 0;
    array = (cnt_word_t*) calloc(K, sizeof(cnt_word_t));
    min = -1;
}

TopKList::~TopKList() {
    free(array);
}

/**
 * inserts into the sorted array. Find pos by binary search, move the elements
 * to the right and insert at pos. The only assumption is that all elements are
 * filled and elements smaller than min are not sent as arguments. Since this
 * is the usual case, tried to keep it short and optimized (no conditional checks)
 */
void TopKList::insert_into_array(const cnt_word_t& cnt_word) {
    packed_t* src = std::upper_bound(&array[0].cnt_top, &array[K].cnt_top,
            cnt_word.cnt_top, cnt_cmp);
    memmove(src + 1, src, (&array[K - 1].cnt_top - src) * sizeof(packed_t));
    *src = cnt_word.cnt_top;
}

void TopKList::insert_word(const cnt_word_t& cnt_word) {
    LOG_IF(FATAL,K==0)<< "Cannot work with Top0Lists. Quitting";

    if(num_elements<K) {
        //		insert_into_array(cnt_word);
        if(num_elements==0) {
            array[0] = cnt_word;
            min = cnt_word.choose.cnt;
            ++num_elements;
            return;
        }
        packed_t* src = std::upper_bound(&array[0].cnt_top,&array[num_elements].cnt_top,cnt_word.cnt_top,cnt_cmp);
        if(src!=&array[num_elements].cnt_top)
        memmove(src+1,src,(&array[num_elements].cnt_top - src)*sizeof(packed_t));
        *src = cnt_word.cnt_top;
        ++num_elements;
        min = array[num_elements-1].choose.cnt;
    }
    else {
        if(cnt_word.choose.cnt>min) {
            insert_into_array(cnt_word);
            min = array[K-1].choose.cnt;
        }
    }
}

    /**
     * Used for test convenience. Just to check the
     * array is always sorted and has at most K elems.
     * Should return true at any point of time
     */
bool TopKList::is_sorted() {
    for (int i = 0; i < num_elements - 1; i++) {
        //		if(!cnt_cmp(array[i].cnt_top,array[i+1].cnt_top))
        if (!(array[i].cnt_top > array[i + 1].cnt_top))
            return false;
    }
    return true && (num_elements <= K);
}

/**
 * Returns the max element which is the first elem
 */
cnt_word_t TopKList::get_max() {
    return array[0];
}

/**
 * Iterator methods. Return an iterator to the
 * beginning of the list
 */
TopKList::iterator TopKList::get_beg() {
    return &array[0];
}

/**
 * Iterator methods. Return an iterator to the
 * end of the list
 */
TopKList::iterator TopKList::get_end() {
    return &array[num_elements];
}

/**
 * Print the list to log(INFO)
 */
void TopKList::print() {
    std::stringstream ss;
    for (int i = 0; i < num_elements; i++) {
        ss << "(" << array[i].choose.top << ", " << array[i].choose.cnt << ") ";
    }
    ss << std::endl;
    LOG(INFO) << ss.str();
    ss.str("");
}

/**
 * Clears the list
 */
void TopKList::clear() {
    memset(array, 0, num_elements * sizeof(cnt_word_t));
    num_elements = 0;
    min = -1;
}
