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
 * Unigram_Model_Synchronizer_Helper.cpp
 *
 *  Created on: 13-Jan-2011
 *      
 */

#include "Unigram_Model_Synchronizer_Helper.h"
#include "Context.h"
#include "TopicLearner/DM_Client.h"

Unigram_Model_Synchronizer_Helper::Unigram_Model_Synchronizer_Helper(
        TypeTopicCounts& ttc, WordIndexDictionary& dict) :
    _ttc(ttc), _dict(dict) {
    _num_words = _ttc.get_num_words();
    _num_topics = _ttc.get_num_topics();
    _prev_ttc = new TypeTopicCounts(_num_words, _num_topics);
    _prev_ttc->initialize_from_ttc(&_ttc);
    _cur_word = 0;
    tc_client.assign(_num_topics);
    tc_old.assign(_num_topics);
    string servers = Context::get_instance().get_string("servers");
    _client = new DM_Client(_num_words, servers.c_str(), *this);
}

Unigram_Model_Synchronizer_Helper::~Unigram_Model_Synchronizer_Helper() {
    delete _prev_ttc;
    delete _client;
}

void Unigram_Model_Synchronizer_Helper::initialize() {
    topicCounts topCnts(_num_topics);
    for (word_t word_ind = 0; word_ind < _num_words; word_ind++) {
        string word = _dict.get_word(word_ind);
        //Step A1
        _ttc.get_counts(word_ind, &topCnts);
        string counts;
        topCnts.convertTo(counts);
        _client->put(word, counts);
        LOG_EVERY_N(WARNING,10000)
            << "Counts of " << google::COUNTER
                    << " words have been transferred to DM_Server.";
    }

    LOG(WARNING)
            << "Local counts written to DM_Server. Phase1 for client complete";
    _client->wait_for_all();
    LOG(WARNING)
            << "All clients have transferred their counts successfully. Global initial counts ready";

    for (word_t word_ind = 0; word_ind < _num_words; word_ind++) {
        string word = _dict.get_word(word_ind);
        _ttc.get_counts(word_ind, &topCnts);
        string counts;
        _client->get(word, counts);
        topicCounts tc_word(counts);
        tc_word -= topCnts;
        mapped_vec delta;
        tc_word.convertTo(delta);
        _ttc.upd_count(word_ind, delta);

        _prev_ttc->upd_count(word_ind, delta);
        LOG_EVERY_N(WARNING,10000)
            << "Global counts of " << google::COUNTER
                    << " words have been retrieved from DM_Server.";
    }
    LOG(WARNING) << "Client initialized successfully";
}

bool Unigram_Model_Synchronizer_Helper::has_to_synchronize() {
    return _cur_word < _num_words;
}

void Unigram_Model_Synchronizer_Helper::reset_to_synchronize() {
    _client->wait_till_done();
    _cur_word = 0;
}

void Unigram_Model_Synchronizer_Helper::synchronize() {
    string word = _dict.get_word(_cur_word);
    //Now ttc(word)_global = tc_old_global = tc_global

    //ToGlobal
    //Find delta local by tc_client - tc_old
    //This is just local because, globals are same
    _ttc.get_counts(_cur_word, &tc_client);
    _prev_ttc->get_counts(_cur_word, &tc_old);
    tc_client -= tc_old;

    mapped_vec delta;
    int sum = tc_client.convertTo(delta);
    if (delta.size() != 0 && sum != 0)
        LOG(WARNING) << "Synch ttc The deltas sum up to " << sum << endl;
    _prev_ttc->upd_count(_cur_word, delta, "");
    //Now tc_old_local = tc_client

    string delta_str;
    tc_client.convertTo(delta_str);
    _client->begin_putNget(word, delta_str);
    ++_cur_word;
}

void Unigram_Model_Synchronizer_Helper::end_putNget(const string& word,
        const string& counts) {
    //FromGlobal
    int word_ind = _dict.get_index(word);
    topicCounts tc_global(counts);
    topicCounts tc_old(_num_topics);
    _prev_ttc->get_counts(word_ind, &tc_old);
    tc_global -= tc_old;
    mapped_vec delta;
    int sum = tc_global.convertTo(delta);
    if (delta.size() != 0 && sum != 0)
        LOG(WARNING) << "Synch ttc The deltas sum up to " << sum << endl;
    _ttc.upd_count(word_ind, delta, "");
    tc_global.convertTo(delta);
    _prev_ttc->upd_count(word_ind, delta, "");
}
