from __future__ import division
import numpy as np
# from sklearn.preprocessing import normalize
import sys


class Perceptron:
    def __init__(self, shape: tuple):
        """
        Init the perceptron and
        initiate random value to the weights

        :param shape: tuple shape of the data
        """
        self.nb_in = shape[1] - 1
        self.nb_out = 1
        self.weight = np.random.rand(self.nb_in + 1, self.nb_out) * 0.1 - 0.05

    def train(self, data: np.ndarray, lr: float, cycles: int):
        """
        Train the perceptron

        :param data: np.ndarray train data
        :param lr: float learning rate
        :param cycles: int number of learning cycles
        """
        in_data = data[:, :-1]
        out_data = data[:, -1:]
        in_data = np.concatenate((in_data, np.ones((in_data.shape[0], 1))), axis=1)
        for i in range(cycles):
            result = self.predict(in_data, False)
            self.weight -= lr * np.dot(np.transpose(in_data), result - out_data)

    def predict(self, in_data: np.ndarray, bias: bool = True):
        """
        Compute the input with the perceptron's weights
        and return its predictions

        :param in_data: nb.ndarray inputs for the prediction
        :param bias: bool add the bias collum if true (default true)
        :return nb.ndarray prediction array
        """
        if bias:
            in_data = np.concatenate((in_data, np.ones((in_data.shape[0], 1))), axis=1)
        result = np.dot(in_data, self.weight)
        return result
        # return np.where(result > 0.5, 1, 0)

    def reset(self, nb_in: int = 0, nb_out: int = 0):
        self.nb_in = (self.nb_in, nb_in)[nb_in != 0]
        self.nb_out = (self.nb_out, nb_out)[nb_out != 0]
        self.weight = np.random.rand(self.nb_in + 1, self.nb_out) * 0.1 - 0.05


class LinearRegression:
    def __init__(self, shape: tuple):
        """

        :param shape: tuple shape of the data
        """
        self.nb_in = shape[1] - 1
        self.nb_out = 1
        self.weight = np.zeros((self.nb_in, self.nb_out))

    def train(self, data: np.ndarray):
        """
        Train with the normal equation

        :param data: np.ndarray train data
        """
        in_data = data[:, :-1]
        out_data = data[:, -1:]
        in_data = np.concatenate((in_data, np.ones((in_data.shape[0], 1))), axis=1)
        self.weight = np.dot(np.dot(np.linalg.inv(np.dot(np.transpose(in_data), in_data)), np.transpose(in_data)), out_data)

    def predict(self, in_data: np.ndarray, bias: bool = True):
        """
        Compute the input with the weights
        and return its predictions

        :param in_data: nb.ndarray inputs for the prediction
        :param bias: bool add the bias collum if true (default true)
        :return nb.ndarray prediction array
        """
        if bias:
            in_data = np.concatenate((in_data, np.ones((in_data.shape[0], 1))), axis=1)
        result = np.dot(in_data, self.weight)
        return result

    def reset(self, nb_in: int = 0, nb_out: int = 0):
        self.nb_in = (self.nb_in, nb_in)[nb_in != 0]
        self.nb_out = (self.nb_out, nb_out)[nb_out != 0]
        self.weight = np.zeros((self.nb_in, self.nb_out))


def sum_square_error(result: np.ndarray, target: np.ndarray):
    return 1/(2 * np.shape(result)[0]) * (np.sum(np.square(result - target)))


if __name__ == "__main__":

    if sys.version_info < (3, 0):
        sys.stdout.write("Warning, code writen for Python 3.x, not Python 2.x\n")
        # sys.exit(1)

    TRAIN_DATA_FILE = "PROSTATE_TRAIN.TXT"
    TEST_DATA_FILE = "PROSTATE_TEST.TXT"

    train_data = np.loadtxt(TRAIN_DATA_FILE)
    test_data = np.loadtxt(TEST_DATA_FILE)

    PC = Perceptron(train_data.shape)
    LR = LinearRegression(train_data.shape)

    PC.train(train_data, 0.01, 25)
    LR.train(train_data)

    print("<=====================>")
    print("square_error on train data")
    print("Perceptron => ", sum_square_error(train_data[:, -1:], PC.predict(train_data[:, :-1])))
    print("Linear Regression => ", sum_square_error(train_data[:, -1], LR.predict(train_data[:, :-1])))
    print("---------------------")
    print("square_error on test data")
    print("Perceptron => ", sum_square_error(test_data[:, -1:], PC.predict(test_data[:, :-1])))
    print("Linear Regression => ", sum_square_error(test_data[:, -1:], LR.predict(test_data[:, :-1])))

    PC.reset()
    LR.reset()

    # train_data = normalize(train_data, axis=0)
    # test_data = normalize(test_data, axis=0)

    train_data = train_data / np.linalg.norm(train_data, ord=2, axis=0, keepdims=True)
    test_data = test_data / np.linalg.norm(test_data, ord=2, axis=0, keepdims=True)

    PC.train(train_data, 0.01, 25)
    LR.train(train_data)

    print("<=====================>")
    print("square_error on normalized train data")
    print("Perceptron => ", sum_square_error(train_data[:, -1:], PC.predict(train_data[:, :-1])))
    print("Linear Regression => ", sum_square_error(train_data[:, -1:], LR.predict(train_data[:, :-1])))
    print("---------------------")
    print("square_error on normalized test data")
    print("Perceptron => ", sum_square_error(test_data[:, -1:], PC.predict(test_data[:, :-1])))
    print("Linear Regression => ", sum_square_error(test_data[:, -1:], LR.predict(test_data[:, :-1])))
    print("<=====================>")