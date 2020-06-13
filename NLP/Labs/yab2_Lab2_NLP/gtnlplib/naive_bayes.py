from gtnlplib.constants import OFFSET
from gtnlplib import clf_base, evaluation

import numpy as np
from collections import defaultdict
from collections import Counter
# deliverable 3.1
def get_corpus_counts(x,y,label):
    """Compute corpus counts of words for all documents with a given label.

    :param x: list of counts, one per instance
    :param y: list of labels, one per instance
    :param label: desired label for corpus counts
    :returns: defaultdict of corpus counts
    :rtype: defaultdict

    """
    out = Counter()

    for (index, Y) in enumerate(y):
        if label == Y:
            out = x[index] + out

    return defaultdict(float, out)


# deliverable 3.2
def estimate_pxy(x,y,label,smoothing,vocab):
    '''
    Compute smoothed log-probability P(word | label) for a given label.

    :param x: list of counts, one per instance
    :param y: list of labels, one per instance
    :param label: desired label
    :param smoothing: additive smoothing amount
    :param vocab: list of words in vocabulary
    :returns: defaultdict of log probabilities per word
    :rtype: defaultdict of log probabilities per word

    '''
    n = len(vocab)

    doclabel = [x[i] for i in range(len(y)) if y[i] == label]

    countlabel = Counter()

    for doc in doclabel:
        countlabel.update(doc)

    wordslen = len(list(countlabel.elements()))

    logp = defaultdict(float)

    sub = np.log(wordslen + (n * smoothing))
    for (v, _) in vocab:
        logp[v] = np.log(countlabel[v] + smoothing) - sub
    return defaultdict(float,logp)


# deliverable 3.3
def estimate_nb(x,y,smoothing):
    """estimate a naive bayes model

    :param x: list of dictionaries of base feature counts
    :param y: list of labels
    :param smoothing: smoothing constant
    :returns: weights
    :rtype: defaultdict 

    """
    
    labels = set(y)
    counts = defaultdict(float)
    doc_counts = defaultdict(float)
    
    labelcount = Counter()
    labelcount.update(y)

    vocabcount = Counter()

    for X in x:
        vocabcount.update(X)

    vocab = set(vocabcount.items())
    weights=defaultdict(float)

    for l in labels:
   
        estp=estimate_pxy(x, y, l, smoothing, vocab)
        for word in estp:
            lol = labelcount[l]/len(y)
            weights[l,word]=estp[word]
            weights[l,OFFSET]=np.log(lol)        

    return defaultdict(float,weights)



# deliverable 3.4
def find_best_smoother(x_tr,y_tr,x_dv,y_dv,smoothers):
    '''
    find the smoothing value that gives the best accuracy on the dev data

    :param x_tr: training instances
    :param y_tr: training labels
    :param x_dv: dev instances
    :param y_dv: dev labels
    :param smoothers: list of smoothing values
    :returns: best smoothing value
    :rtype: float

    '''

    labels = list(set(y_tr))

    bestval = 0
    bestsmoother = None
    scores = {}

    for s in smoothers:
        est = estimate_nb(x_tr, y_tr, s)
        Y = clf_base.predict_all(x_dv, est, labels)
        val = evaluation.acc(Y, y_dv)
        scores[s] = val
        if val > bestval:
            bestval = val
            bestsmoother = s

    return bestsmoother, scores







