class Point:
    def __init__(self, x, y):
        self.point = (x, y)

    def __str__(self):
        return str(self.point)
    def __repr__(self):
        return str(self.point)

class Hand:
    def __init__(self, *args):
        self.points = []
        for point in args:
            self.points.append(point)
        print(self)
    def __str__(self):
        return str(self.points)
    def __repr__(self):
        return str(self.points)


def testLoaded():
    return True
