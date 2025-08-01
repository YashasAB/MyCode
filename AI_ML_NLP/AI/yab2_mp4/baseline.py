# mp4.py
# ---------------
# Licensing Information:  You are free to use or extend this projects for
# educational purposes provided that (1) you do not distribute or publish
# solutions, (2) you retain this notice, and (3) you provide clear
# attribution to the University of Illinois at Urbana-Champaign
#
# Created Fall 2018: Margaret Fleck, Renxuan Wang, Tiantian Fang, Edward Huang (adapted from a U. Penn assignment)
# Modified Spring 2020: Jialu Li, Guannan Guo, and Kiran Ramnath
# Modified Fall 2020: Amnon Attali, Jatin Arora
# Modified Spring 2021 by Kiran Ramnath
"""
Part 1: Simple baseline that only uses word statistics to predict tags
"""
from collections import Counter

class wcount: 
    
    def __init__(self):
        self.tags = Counter()
         
    def addtag(self, tag):
        
        self.tags.update([tag])
            
    def maxtag(self):
        x = self.tags.most_common(1)
        return x[0][0]
            
   
            

def baseline(train, test):
    '''
    input:  training data (list of sentences, with tags on the words)
            test data (list of sentences, no tags on the words)
    output: list of sentences, each sentence is a list of (word,tag) pairs.
            E.g., [[(word1, tag1), (word2, tag2)], [(word3, tag3), (word4, tag4)]]
    '''
    words = {}
    types = Counter()
    
    for sent in train:
        
        for pair in sent:
            
            w,t = pair
           
            types.update([t])
            
            #returns count of all tags for a word
            tcount = words.get(w, -1)
            
            if tcount==-1:
                a = wcount()
                a.addtag(t)
                words[w] = a
            else:          
                tcount.addtag(t)
                
    output = []
    unknowntag = types.most_common(1)[0][0]
    
    for sent in test:
        out=[]
        for w in sent:
            tags = words.get(w, -1)
            
            if tags==-1:
                out.append((w,unknowntag))
                
            else:
                t = tags.maxtag()
                out.append((w,t))
       
        output.append(out)
        
    
    return output