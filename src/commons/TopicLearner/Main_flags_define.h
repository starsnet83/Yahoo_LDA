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
 * Main_flags_define.h
 *
 * Flags for learntopics
 *
 *  Created on: 19 Jul, 2009
 *      
 */

#ifndef MAIN_FLAGS_DEFINE_H_
#define MAIN_FLAGS_DEFINE_H_

#include "gflags/gflags.h"
#include "constants.h"
#include "tbb/task_scheduler_init.h"

DEFINE_int32(iter,1000,"Number of iterations the topic modeller should be run");
DEFINE_int32(burnin,299,"Number of iterations after which alpha optimization should be to be run after every <optimizestats> iterations");
DEFINE_int32(optimizestats,25,"Optimize hyper parameters every these many iterations");
DEFINE_int32(printloglikelihood,25,"Print log likelihood after every <printlogLikelihood> iterations after burn-in");
DEFINE_int32(topics,100,"The number of topics to be used by LDA.");

//dataflow
DEFINE_string(inputprefix,"lda","The output prefix used for the FormatData routine");
DEFINE_string(dumpprefix,"","The word-topic counts are initialized from this file which is generated by the preprocessing step or at the end of an iteration");

//parameters
DEFINE_bool(restart,false,"Indicates use of failure recovery mode. The iteration to start with should also be specified");
DEFINE_bool(online,false,"Uses online initialization instead of random");
DEFINE_int32(startiter,1,"This the iteration at which failure recovery should start");
DEFINE_bool(test,false,"Run the test pipeline. No updates are done & requires an earlier dump of the word-topic counts table");
DEFINE_bool(teststream,false,"Run the test pipeline in streaming mode. Formatting is a part of the pipeline. No updates are done & requires an earlier dump of the word-topic counts table & dictionary");
DEFINE_double(alpha,ALPHA_SUM,"Weight of the Dirichlet conjugate for topics");
DEFINE_double(beta,BETA,"Weight of the Dirichlet conjugate for words");
DEFINE_int32(chkptinterval,25,"The topic assignments are saved every these many iterations");
DEFINE_string(chkptdir,"","The directory to which the checkpoints need to written");
DEFINE_string(servers,"specify","The set of all memcached servers that are storing the state. E.g. 192.168.0.1, 192.168.0.3:44, 200.132.12.34");
DEFINE_int32(numdumps,1,"Number of word-topic count dumps in the training data");
DEFINE_int32(maxmemory,2048,"The max memory that can be used");
DEFINE_string(dictionary,"specify","The dump of the global dictionary produced in the training run. To be use for teststream");
//hidden
DEFINE_int32(livetokens,500,"Max Live Tokens in pipeline");
DEFINE_int32(model,1,"Unigram-1");
DEFINE_int32(samplerthreads,tbb::task_scheduler_init::automatic,"The number of foreground threads that run actual LDA pipeline. Default is to figure out automatically");
#endif /* MAIN_FLAGS_DEFINE_H_ */
