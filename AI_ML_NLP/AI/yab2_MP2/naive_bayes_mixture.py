# naive_bayes_mixture.py
# ---------------
# Licensing Information:  You are free to use or extend this projects for
# educational purposes provided that (1) you do not distribute or publish
# solutions, (2) you retain this notice, and (3) you provide clear
# attribution to the University of Illinois at Urbana-Champaign
#
# Created by Justin Lizama (jlizama2@illinois.edu) on 09/28/2018
# Modified by Jaewook Yeom 02/02/2020

"""
This is the main entry point for Part 2 of this MP. You should only modify code
within this file for Part 2 -- the unrevised staff files will be used for all other
files and classes when code is run, so be careful to not modify anything else.
"""

import numpy as np
import nltk
nltk.download('punkt')
from nltk.util import ngrams
from nltk.lm import NgramCounter
from nltk import word_tokenize 
import re


def naiveBayesMixture(train_set, train_labels, dev_set, bigram_lambda,unigram_smoothing_parameter, bigram_smoothing_parameter, pos_prior):
    """
    train_set - List of list of words corresponding with each email
    example: suppose I had two emails 'like this movie' and 'i fall asleep' in my training set
    Then train_set := [['like','this','movie'], ['i','fall','asleep']]

    train_labels - List of labels corresponding with train_set
    example: Suppose I had two emails, first one was ham and second one was spam.
    Then train_labels := [1, 0]

    dev_set - List of list of words corresponding with each email that we are testing on
              It follows the same format as train_set

    bigram_lambda - float between 0 and 1

    unigram_smoothing_parameter - Laplace smoothing parameter for unigram model (between 0 and 1)

    bigram_smoothing_parameter - Laplace smoothing parameter for bigram model (between 0 and 1)

    pos_prior - positive prior probability (between 0 and 1)
    """

    # TODO: Write your code here
    # return predicted labels of development set
    
    
    usp = unigram_smoothing_parameter

    bsp = bigram_smoothing_parameter

    lamda = bigram_lambda

    prior = pos_prior

    bhamcounts = NgramCounter()

    bspamcounts = NgramCounter()

    hamcounts = NgramCounter()

    spamcounts = NgramCounter()


    Hcunt = {}
    Scunt = {}
    
    
    
   
    
    for i in range(0,train_labels.shape[0]):

        mess = ' '.join(x for x in train_set[i] )
        mess.lower()
        
        message = re.sub(r'd+', '', mess)
        
        if train_labels[i]==1:

            hamcounts.update([ngrams(word_tokenize(message), 1)])
            bhamcounts.update([ngrams(word_tokenize(message), 2)])


        if train_labels[i]==0:

            spamcounts.update([ngrams(word_tokenize(message), 1)])
            bspamcounts.update([ngrams(word_tokenize(message), 2)])

    biprob = []
    
    bhden = bhamcounts[2].N()+(bsp*len(bhamcounts[2].conditions()))
    bsden = bspamcounts[2].N()+(bsp*len(bspamcounts[2].conditions()))
    predicts = []
    for mes in dev_set:

        pham = 0
        pspam = 0
        
        mess = ' '.join(x for x in mes)
        mess.lower()
        
        msg = re.sub(r'd+', '', mess)
        
        
        for bi in ngrams(word_tokenize(msg), 2):

            h = Hcunt.get((bi[0],bi[1]), -1)

            if h>= 0:
                hlike = Hcunt[bi[0], bi[1]]

            else: 
                hcount = bhamcounts[[bi[0]]][bi[1]]
                hlike = (hcount+bsp)/(bhden)
                Hcunt[bi[0], bi[1]] = hlike

            s = Scunt.get((bi[0],bi[1]), -1)

            if s>= 0:
                slike = Scunt[bi[0], bi[1]]
            else: 
                scount = bspamcounts[[bi[0]]][bi[1]]
                slike = (scount+bsp)/(bsden)
                Scunt[bi[0], bi[1]] = slike

            pham = pham + np.log10(hlike)

            pspam = pspam + np.log10(slike)



#         pham = pham + (np.log10(prior)  * 1.15 *lamda)

#         pspam = pspam + (np.log10(1-prior) * 0.95 *  lamda)

        biprob.append((pham, pspam))


    uprob = uniprobs(hamcounts, spamcounts, dev_set, usp, prior, lamda)

    predicts = []

    for i in range(0, uprob.shape[0]):
        
        if(prior<=0.5):

            pham = uprob[i][0]/1.2 + biprob[i][0] + (np.log(prior) * (1-lamda)) + (np.log10(prior) * lamda)

            pspam = uprob[i][1]/1.24 + biprob[i][1] + (np.log10(1-prior) * lamda) + (np.log10(1-prior) *(1-lamda))
        else:
            pham = uprob[i][0]/1.275 + biprob[i][0] + (np.log(prior) * (1-lamda)) + (np.log10(prior)  *lamda)

            pspam = uprob[i][1]/1.2 + biprob[i][1] + (np.log10(1-prior) * lamda) + (np.log10(1-prior) * 3.3 * (1-lamda))

        if pspam>pham:
            predicts.append(0)
        else:
            predicts.append(1)

        


    predicted_labels = np.asarray(predicts)

    
    return predicted_labels



def uniprobs(hamcounts, spamcounts, dev_set, usp, prior, lamda):
    
    probs = []

    hden = hamcounts.N()+(usp*hamcounts[1].B())
    sden = spamcounts.N()+(usp*spamcounts[1].B())
    
    num = 0
    for mes in dev_set:

        pham = 0
        pspam = 0

        for word in mes:

            hlike = (hamcounts[word]+usp)/(hden)

            slike = (spamcounts[word]+usp)/(sden)

            pham = pham + np.log10(hlike)

            pspam = pspam + np.log10(slike)

#         pham = pham + (np.log(prior) * (1-lamda))

#         pspam = pspam + (np.log(1-prior) *(1-lamda))
        
        probs.append((pham,pspam))
        
    p = np.asarray(probs)
    
    return p



