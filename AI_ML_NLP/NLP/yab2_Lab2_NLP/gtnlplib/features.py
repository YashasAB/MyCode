from gtnlplib.constants import OFFSET
import numpy as np
import torch

# deliverable x.x
def get_top_features_for_label_numpy(weights,label,k=5):
    '''
    Return the five features with the highest weight for a given label.

    :param weights: the weight dictionary
    :param label: the label you are interested in 
    :returns: list of tuples of features and weights
    :rtype: list
    '''

    raise NotImplementedError


# deliverable 5.1
def get_top_features_for_label_torch(model,vocab,label_set,label,k=5):
    '''
    Return the five words with the highest weight for a given label.

    :param model: PyTorch model
    :param vocab: vocabulary used when features were converted
    :param label_set: set of ordered labels
    :param label: the label you are interested in 
    :returns: list of words
    :rtype: list
    '''

    vocab = sorted(vocab)
    
    weights = list(model.parameters())[0].data.numpy()

    indices = np.argpartition(weights[label_set.index(label), :], -k)[-k:]
    final_list = []
    for i in indices:
        final_list.append((vocab[i], weights[label_set.index(label), i]))

    final_list.sort(key=lambda x: x[1], reverse=True)

    return [i[0][0] for i in final_list]
    

# deliverable 7.1
def get_token_type_ratio(counts):
    '''
    compute the ratio of tokens to types

    :param counts: bag of words feature for a song, as a numpy array
    :returns: ratio of tokens to types
    :rtype: float

    '''
    
    raise NotImplementedError

# deliverable 7.2
def concat_ttr_binned_features(data):
    '''
    Discretize your token-type ratio feature into bins.
    Then concatenate your result to the variable data

    :param data: Bag of words features (e.g. X_tr)
    :returns: Concatenated feature array [Nx(V+7)]
    :rtype: numpy array

    '''
    
    raise NotImplementedError
