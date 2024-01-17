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

from collections import Counter
import numpy as np
import copy
import math

def viterbi_ec(train, test):
    '''
    input:  training data (list of sentences, with tags on the words)
            test data (list of sentences, no tags on the words)
    output: list of sentences with tags on the words
            E.g., [[(word1, tag1), (word2, tag2)], [(word3, tag3), (word4, tag4)]]
    '''
    alpha=0.00001
    words = {}
    types = Counter()
    pairprob = {}
    triprob = {}
    output = []
    suffix = ['ing', 'ly', 'ed', 'ious', 'ies', 'ive', 'es', 's', 'ment']

    suff = {}

    for s in suffix:
        suff[s] = wcount()
    for sent in train:

        prev = ''

        for i in range(0, len(sent)):

            w,t = sent[i]

            types.update([t])

            if i ==0:

                prev = 'START'
                prev3 = 'START'

            else:
                pairs = pairprob.get(prev, -1)

                if pairs== -1:
                    a = wcount()
                    a.addtag(t)
                    pairprob[prev] = a
                else:
                    pairs.addtag(t)
                if i >1:
                    pairs = triprob.get((prev3, prev), -1)
                    if pairs== -1:
                        a = wcount()
                        a.addtag(t)
                        triprob[(prev3,prev)] = a
                    else:
                        pairs.addtag(t)

                    prev3 = prev

                prev = t

            #tcount is counter with number of times each tag occured for a given word
            tcount = words.get(w, -1)

            if tcount==-1:
                a = wcount()
                a.addtag(t)
                words[w] = a
                for s in suffix:
                    if w.endswith(s):
                        suff[s].addtag(t)

            else:
                tcount.addtag(t)
                for s in suffix:
                    if w.endswith(s):
                        suff[s].addtag(t)

#     training is done, predicting below
    tags = []
    tagsend=[]
    for t in types.keys():
        tags.append(t)
        tagsend.append(t)

    tags.remove('START')
    tags.remove('END')
    tagsend.remove('START')


    lentags = len(tags)

    for t in tags:
        pairprob['START'].probs[t] = math.log((pairprob['START'].tags[t] + alpha) / (types['START'] + len(tags) * alpha), 10)

        for tt in tagsend:
            x = triprob.get(('START',t),-1)
            if x == -1:
                a = wcount()
                a.probs[tt] = math.log((a.tags[tt] + alpha) / (types['START'] + len(tags) * alpha), 10)
                triprob[('START',t)] = a
            else:
                triprob[('START',t)].probs[tt] = math.log((triprob[('START',t)].tags[tt] + alpha) / (types['START'] + len(tags) * alpha), 10)



    for t in tags:
        for tt in tags:
            for ttt in tagsend:
                x = triprob.get((t,tt),-1)
                if x==-1:
                    a = wcount()
                    a.probs[ttt] = math.log((a.tags[ttt] + alpha) / (types[t] + len(tags) * alpha), 10)
                    triprob[(t,tt)] = a
                else:
                    triprob[(t,tt)].probs[ttt] = math.log((triprob[(t,tt)].tags[ttt] + alpha) / (types[t] + len(tags) * alpha), 10)

    hxprob = wcount()
    hxwords = 0


    for word in words.keys():
        if sum(words[word].tags.values()) == 1:
            t = words[word].maxtag()
            hxprob.addtag(t)
            hxwords += 1

    thapax = hxwords

    for t in tags:
        hxprob.probs[t] = math.log((hxprob.tags[t]+alpha)/(thapax+(alpha*(lentags+1))), 10)


    for i in range(0, len(tags)):
        if tags[i] not in pairprob.keys():
            a = wcount()
            for t in tagsend:
                a.probs[t]=math.log(alpha / (types[t] + (alpha*(lentags+1))), 10)
            pairprob[tags[i]] = a
        else:
            for t in tagsend:
                pairprob[tags[i]].probs[t]=math.log((pairprob[tags[i]].tags[t] + alpha) / (types[t] + (alpha*(lentags+1))), 10)

#     for i in range(0, len(tagsend)):
#         for t in tagsend:
#             k = triprob.get((tagsend[i],t),-1)
#             if k ==-1:
#                 a = wcount()
#                 for t in tags:
#                     a.probs[t]=np.log(alpha / (types[t] + (alpha*(lentags+1))))
#                 triprob[(tagsend[i],t)] = a
#             else:
#                 for tt in tagsend:
#                     triprob[(tagsend[i],t)].probs[tt]=np.log((triprob[(tagsend[i],t)].tags[tt] + alpha) / (types[tt] + (alpha*(lentags+1))))


#         create nodes for all tags
    nodes = []
    for t in tags:
        n = tnode()
        nodes.append(n)

    lenwords = len(words.keys())

    wprobs={}
    endprobs = calcendprobs(pairprob, tags)
    endprobs3 = calcendprobs3(triprob)

    for s in suffix:
        for t in tags:
            suff[s].probs[t]=math.log((suff[s].tags[t] + alpha) / (types[t] + (alpha*(lentags+1))), 10) + hxprob.probs[t]

    unk = set()
    tlen = len(test)

    for sent in test:
        trell = trellis(nodes)
        unseennum = []
        unseenadj = []
        unknown = 0

        for i in range(1, len(sent)-1):

            w = sent[i]

            p = wprobs.get(w,-1)

            if p==-1:
                x = words.get(w, -1)
                if x == -1:
                    pro = {}
                    unknown = 1
                    unk.add(w)
                    for t in tags:
                        pro[t] = hxprob.probs[t] + math.log((alpha / (types[t] + (lentags+1)*alpha)), 10)
                    wprobs[w] = pro
                    if len(w) < 2:
                        if w[0].isdigit():
                            unseennum.append(i)
                    else:
                        if w[0]=='$':
                            unseenadj.append(i)
                        elif w[0].isdigit():
                            if w[1].isdigit() or w[1] == '.':
                                unseennum.append(i)

                        else:
                            for s in suffix:
                                if w.endswith(s):
                                    wprobs[w] = suff[s].probs
                                    break
                else:
                    for t in tags:
                        x.probs[t] = math.log((x.tags[t] + alpha) / (types[t] + (lenwords+1)*alpha), 10)
                    if w in unk:
                        unknown = 1
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
                for x in range(0, len(tags)):
                    if len(trell.curr[x].path)!=0:
                        trell.curr[x].prob = trell.curr[x].prob + (0.3*endprobs[tags[x]]) + (0.7*endprobs3[(trell.curr[x].path[-1],tags[x])])
                    else:
                        trell.curr[x].prob = trell.curr[x].prob + (0.001 * endprobs[tags[x]])
                trell.update()

            else:
            #dealing with the second word onwards
                if i!=2:
                    if tlen < 5000:
                        trell.curr = calctrelnodes32(trell.prev,wprobs[w], tags, pairprob, triprob)
                    else:
                        trell.curr = calctrelnodes3(trell.prev,wprobs[w], tags, pairprob, triprob)
                    trell.update()
                else:
                    trell.curr = calctrelnodes(trell.prev,wprobs[w], tags, pairprob)
                    trell.update()
                # if unknown ==1:
                #     if i!=2:
                #         if tlen < 5000:
                #             trell.curr = calctrelnodes32(trell.prev,wprobs[w], tags, pairprob, triprob)
                #         else:
                #             trell.curr = calctrelnodes3(trell.prev,wprobs[w], tags, pairprob, triprob)
                #
                #     else:
                #         trell.curr = calctrelnodes(trell.prev,wprobs[w], tags, pairprob)
                #         trell.update()
                #
                # else:
                #     trell.curr = calctrelnodes(trell.prev,wprobs[w], tags, pairprob)
                #     trell.update()

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
        for n in unseennum:
            out[n] = (out[n][0],'NUM')
        for n in unseenadj:
            out[n]= (out[n][0], 'ADJ')

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

def calctrelnodes3(prev, wtprobs, tags, pairprobs, triprobs):
    out = []
    for i in range(0, len(tags)):
        n = tnode()
        n.prob = wtprobs[tags[i]]
        nprob = []
        for j in range(0, len(tags)):
            if tags[j] != 'END':
                if len(prev[j].path)!=0:
                    x = prev[j].prob +  (0.3 * triprobs[(prev[j].path[-1], tags[j])].probs[tags[i]]) +  (pairprobs[tags[j]].probs[tags[i]] * 0.7) + n.prob
                else:
                    x = prev[j].prob +  (0.001 * pairprobs[tags[j]].probs[tags[i]]) + n.prob
                nprob.append(x)

        nprob = np.asarray(nprob)
        b = np.argmax(nprob)
        n.prob = nprob[b]
        n.path.extend(prev[b].path)
        n.path.append(tags[b])

        out.append(n)
    return out

def calctrelnodes32(prev, wtprobs, tags, pairprobs, triprobs):
    out = []
    for i in range(0, len(tags)):
        n = tnode()
        n.prob = wtprobs[tags[i]]
        nprob = []
        for j in range(0, len(tags)):
            if tags[j] != 'END':
                if len(prev[j].path)!=0:
                    x = prev[j].prob +  (0.75 * triprobs[(prev[j].path[-1], tags[j])].probs[tags[i]]) +  (pairprobs[tags[j]].probs[tags[i]] * 0.25) + n.prob
                else:
                    x = prev[j].prob +  (0.001 * pairprobs[tags[j]].probs[tags[i]]) + n.prob
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

def calcendprobs3(triprobs):
    probs = {}
    structs = []
    for t in triprobs.keys():
        probs[t] = triprobs[t].probs['END']

    return probs



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
