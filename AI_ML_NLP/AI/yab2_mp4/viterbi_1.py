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
# Modified Spring 2021 by Kiran Ramnath (kiranr2@illinois.edu)

"""
Part 2: This is the simplest version of viterbi that doesn't do anything special for unseen words
but it should do better than the baseline at words with multiple tags (because now you're using context
to predict the tag).
"""



def viterbi_1(train, test):
    '''
    input:  training data (list of sentences, with tags on the words)
            test data (list of sentences, no tags on the words)
    output: list of sentences with tags on the words
            E.g., [[(word1, tag1), (word2, tag2)], [(word3, tag3), (word4, tag4)]]
    '''
    alpha=0.001
    words = {}
    types = Counter()
    pairprob = {}
    output = []
    for sent in train:
        
        prev = ''
        
        for i in range(0, len(sent)):
            
            w,t = sent[i]
            
            types.update([t])
            
            if i ==0:
                
                prev = 'START'
                
            else:
                pairs = pairprob.get(prev, -1)
                
                if pairs== -1:
                    a = wcount()
                    a.addtag(t)
                    pairprob[prev] = a
                else:
                    pairs.addtag(t)
                
                prev = t
                                      
            #tcount is counter with number of times each tag occured for a given word
            tcount = words.get(w, -1)
            
            if tcount==-1:
                a = wcount()
                a.addtag(t)
                words[w] = a
            else:
                tcount.addtag(t)
                
#     training is done, predicting below
    tags = []
    for t in types.keys():
        tags.append(t) 
        
    tags.remove('START')
    
    lentags = len(tags)
    
    for t in tags:   
        pairprob['START'].probs[t] = np.log((pairprob['START'].tags[t] + alpha) / (types['START'] + len(tags) * alpha))
    
    hxprob = wcount()
    hxwords = 0
    
    
    
    for word in words.keys():
        if sum(words[word].tags.values()) == 1:
            t = words[word].maxtag()            
            hxprob.addtag(t)
            hxwords += 1
    
    thapax = hxwords
      
    for t in tags:
        hxprob.probs[t] = np.log((hxprob.tags[t]+alpha)/(thapax+(alpha*(lentags+1))))
        
    
    for i in range(0, len(tags)):
        if tags[i] not in pairprob.keys():
            a = wcount()
            for t in tags:
                a.probs[t]=np.log(alpha / (types[t] + (alpha*(lentags+1))))            
            pairprob[tags[i]] = a
        else:
            for t in tags:
                pairprob[tags[i]].probs[t]=np.log((pairprob[tags[i]].tags[t] + alpha) / (types[t] + (alpha*(lentags+1))))   
                               
    endprobs = calcendprobs(pairprob, tags)
    
#         create nodes for all tags
    nodes = []
    for t in tags:
        n = tnode()
        nodes.append(n)
        
    lenwords = len(words.keys())
    
    wprobs={}
    
    for sent in test:
        trell = trellis(nodes)
        for i in range(1, len(sent)-1):
                    
            w = sent[i]
            
            p = wprobs.get(w,-1)
            
            if p==-1:
                x = words.get(w, -1)
                if x == -1:
                    pro = {}
                    for t in tags:
                        pro[t] = hxprob.probs[t] + np.log((alpha / (types[t] + (lentags+1)*alpha)))
                    wprobs[w] = pro
                                        
                else:
                    for t in tags:
                        x.probs[t] = np.log((x.tags[t] + alpha) / (types[t] + (lenwords+1)*alpha))                    
                    wprobs[w] = x.probs
                    
            #now p has the word dict- keys = tags, val = prob that word has that tag
            if i ==1:
                #if dealing with first word of sentance. trell.prevs nodes should have start probs
                #start probs = prob word has tag t * prob that tag t is at start
                
                sprob = pairprob['START'].probs 
                for j in range(0, lentags):                    
                    trell.prev[j].prob = sprob[tags[j]]+ wprobs[w][tags[j]] 
                    
            # deafling with last word
            elif i==(len(sent)-2):
                
                trell.curr = calctrelnodes(trell.prev,wprobs[w], tags, pairprob)
                for j in range(0,len(tags)):                    
                    trell.curr[j].prob = trell.curr[j].prob + endprobs[tags[j]]
                trell.update()
                
            else:
            #dealing with the second word onwards 
                trell.curr = calctrelnodes(trell.prev,wprobs[w], tags, pairprob)                
                trell.update()
                    
#         now done dealing with all words return greatest
        mprob = -100000
        path = []
        for i in range(0, len(tags)):
            n = trell.curr[i]
            if n.prob > mprob:
                mprob = n.prob
                pat=[]
                pat.extend(n.path)
                pat.append(tags[i])
                path = pat
                
        out = [('START', 'START')]
        
        for i in range(0, len(path)):            
            out.append((sent[i+1], path[i]))
            
        out.append(('END', 'END'))
        output.append(out)
        
    
    return output

def calctrelnodes(prev, wtprobs, tags, pairprobs):
    out = []
    for i in range(0, len(tags)):
        n = tnode()
        n.prob = wtprobs[tags[i]]
        nprob = []        
        for j in range(0, len(tags)):
            x = prev[j].prob + pairprobs[tags[j]].probs[tags[i]]+n.prob                
            nprob.append(x)
            
           
        nprob = np.asarray(nprob)
        b = np.argmax(nprob)
        n.prob = nprob[b]
        n.path.extend(prev[b].path)
        n.path.append(tags[b])
        
        out.append(n)
    return out

#returns dict of probs that a tag occurs at the end
def calcendprobs(pairprobs, tags):
    probs = {}
    structs = []  
    for t in tags:
        probs[t] = pairprobs[t].probs['END']
    
    return probs 


from collections import Counter
import numpy as np


class wcount: 
    
    def __init__(self):
        self.tags = Counter()
        self.probs = {}
        
    def addtag(self, tag):
        
        self.tags.update([tag])
            
    def maxtag(self):
        x = self.tags.most_common(1)
        return x[0][0]
    
    
            
            
class trellis: 
    
    def __init__(self, nodes):        
        self.prev = nodes
        self.curr = nodes
    
    def update(self):
        self.prev = self.curr
        
        
class tnode:     
    def __init__(self):
        self.path = []
        self.prob = 0        


            