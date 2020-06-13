import nltk
from nltk.tokenize import sent_tokenize, word_tokenize
from collections import defaultdict
from nltk.lm import Laplace
from nltk.lm.preprocessing import padded_everygram_pipeline
from ece365lib import evaluate
from nltk.util import ngrams
from nltk.probability import FreqDist
# deliverable 1.1
def tokenize_corpus(corpus):
    '''
    Returns the tokenized version of the nltk corpus string.
    
    :param corpus: str, corpus
    :returns: tokenized version of corpus
    :rtype: list of list of strings
    
    Hint: use nltk.tokenize.sent_tokenize, and nltk.tokenize.word_tokenize
    '''
    
    sent=sent_tokenize(corpus)
    lol=[]
    for s in sent:
        lol.append(word_tokenize(s))

    return lol
    

# deliverable 1.2
def pad_corpus(corpus):
    '''
    Returns a padded version of the tokenized corpus.
    
    :param corpus: list of list of str, tokenized corpus.
    :returns: padded version of the tokenized corpus.
    :rtype: list of list of strings
    '''
    corp = [i for i in corpus]
    start_symbol = '<s>'
    end_symbol = '</s>'
    
    for sent in corp:
        sent.insert(0,start_symbol)       
        sent.append(end_symbol)
    

    return corp
    

# deliverable 1.3    
def split_corpus(corpus):
    '''
    Splits the input corpus into a train and test corpus based on a 80-20 split.
    
    :param corpus: list of list of str, padded tokenized corpus.
    :returns: train subset of the corpus.
    :returns: test subset of the corpus.
    :rtype: list of list of strings, list of list of strings
    '''

    train =corpus[:int(0.8*len(corpus))]
    test=corpus[int(0.8*len(corpus)):]
                  
    return train,test

# deliverable 1.4    
def count_ngrams(corpus, n=3):
    '''
    Takes in a corpus and counts the frequency of all unique n-grams (1-grams, 2-grams, ..., up to length n), and stores them in a dictionary. It also returns a list of all unique words (vocab).
    
    :param corpus: list of list of str, padded tokenized training corpus.
    :param n: maximum order of n-grams considered.
    :returns: dictionary of count of n-grams. Keys are n-grams (tuples), and values are their frequency in the corpus.
    :returns: list of vocab words
    :rtype: dictionary, list of strings
    '''
    lol=[]
    unigrams = [ngrams(sent, 1) for sent in corpus]
    for u in unigrams:
        for item in u:
            lol.append(item)
            
    bigrams = [ngrams(sent, 2) for sent in corpus]
    for b in bigrams:
        for item in b:
            lol.append(item)
            
    trigrams = [ngrams(sent, 3) for sent in corpus]
    for t in trigrams:
        for item in t:
            lol.append(item)
            
    LOL=set(lol)
    LOL=list(LOL)
    
    frequency=[]
    fd=FreqDist(lol)
    for w in LOL:
        frequency.append(fd.freq(w)*len(lol))
    
    ncount={k: v for k, v in zip(LOL, frequency)}
    
    voc=[]
    for c in corpus:
        for word in c:
            voc.append(word)
    vocab=list(set(voc))
    
    return dict(ncount),vocab



# deliverable 1.5    
def estimate(counts, word, context):
    '''
    Estimates the n-gram probability of a word [w_i] following a context of size n-1.
    
    :param counts: a dictionary of n-gram counts.
    :param word: a list of one word, [w_i]
    :param context: a list of preceding n-1 words in order
    :returns: probability of the n-gram.
    :rtype: float.
    '''
    
    concount=counts[tuple(context)]
    wordcount=counts[tuple(context+word)]
    prob=wordcount/concount
    
    return prob
    

# deliverable 3.1    
def vary_ngram(train_corpus, test_corpus, n_gram_orders):
    '''
    Returns a dictionary of perplexity values at different order n-gram LMs
    
    :param train_corpus: list of list of str, corpus to train language model on.
    :param test_corpus: list of list of str, corpus to test language model on.
    :n_gram_orders: list of ints, orders of n-grams desired.
    :returns: a dictionary of perplexities at different orders, key=order, value=perplexity.
    :rtype: dict.
    
    Hint: Follow the same LM training procedure as in the notebook in the end of Exercise 1. 
    '''
    
    perp={}
    
    for n in n_gram_orders:
        train, vocab = padded_everygram_pipeline(n, train_corpus)
        lap=Laplace(n)
        lap.fit(train,vocab)
        test = sum([['<s>'] + x + ['</s>'] for x in test_corpus],[])
        p=lap.perplexity(test)
        perp[n]=p
        
    return perp
    
    
    
