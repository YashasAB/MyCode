# naive_bayes.py
# ---------------
# Licensing Information:  You are free to use or extend this projects for
# educational purposes provided that (1) you do not distribute or publish
# solutions, (2) you retain this notice, and (3) you provide clear
# attribution to the University of Illinois at Urbana-Champaign
#
# Created by Justin Lizama (jlizama2@illinois.edu) on 09/28/2018

"""
This is the main entry point for Part 1 of this MP. You should only modify code
within this file for Part 1 -- the unrevised staff files will be used for all other
files and classes when code is run, so be careful to not modify anything else.
"""

import numpy as np
import nltk
nltk.download('punkt')
from nltk.util import ngrams
from nltk.lm import NgramCounter
from nltk import word_tokenize 

def naiveBayes(train_set, train_labels, dev_set, smoothing_parameter, pos_prior):
    """
    train_set - List of list of words corresponding with each email
    example: suppose I had two emails 'like this movie' and 'i fall asleep' in my training set
    Then train_set := [['like','this','movie'], ['i','fall','asleep']]

    train_labels - List of labels corresponding with train_set
    example: Suppose I had two emails, first one was ham and second one was spam.
    Then train_labels := [1, 0]

    dev_set - List of list of words corresponding with each email that we are testing on
              It follows the same format as train_set

    smoothing_parameter - The smoothing parameter --laplace (1.0 by default)
    pos_prior - positive prior probability (between 0 and 1)
    """
    # TODO: Write your code here
    # return predicted labels of development set
    
    sp = smoothing_parameter

    prior = pos_prior

    hamcounts = NgramCounter()


    spamcounts = NgramCounter()

    


    for i in range(0,train_labels.shape[0]):

        message = ' '.join(x for x in train_set[i])
        if train_labels[i]==1:
            hamcounts.update([ngrams(word_tokenize(message), 1)])

        if train_labels[i]==0:
            spamcounts.update([ngrams(word_tokenize(message), 1)])

        
    hden = hamcounts.N()+(sp*hamcounts[1].B())
    sden = spamcounts.N()+(sp*spamcounts[1].B())

    predicts = []

    for mes in dev_set:

        pham = 0
        pspam = 0

        for word in mes:

            hlike = (hamcounts[word]+sp)/(hden)

            slike = (spamcounts[word]+sp)/(sden)

            pham = pham + np.log(hlike)

            pspam = pspam + np.log(slike)

        pham = pham + np.log(prior)

        pspam = pspam + np.log(1-prior)

        if pspam>pham:
            predicts.append(0)
        else:
            predicts.append(1)

    predicted_labels = np.asarray(predicts)

    
    return predicted_labels
    