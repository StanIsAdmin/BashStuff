from random import randint, random
from matplotlib import pyplot as plt
from matplotlib.colors import ListedColormap
import numpy as np
from operator import itemgetter


class Board:
    def __init__(self, size, payoff_matrix, diagonal_neighbours=True, pure_imitation=True):
        self.neighbour_offsets = [(-1, 0), (0, -1), (0, 1), (1, 0)]
        if diagonal_neighbours:
            self.neighbour_offsets.extend([(-1, -1), (-1, 1), (1, -1), (1, 1)])
        self.size = size
        self.payoff_matrix = payoff_matrix
        self.actions = [[randint(0, 1) for _ in range(size)] for _ in range(size)]
        self.payoffs = [[0 for _ in range(size)] for _ in range(size)]
        self.cooperation_levels = [self.get_coop_level()]
        self.pure_imitation = pure_imitation
        self.replicator_divisor = len(self.neighbour_offsets) * (max((max(v for v in row) for row in payoff_matrix)) -
                                                                 min((min(v for v in row) for row in payoff_matrix)))

    def get_coop_level(self):
        cooperators = 0
        for row in range(self.size):
            cooperators += self.actions[row].count(1)
        return cooperators / self.size**2

    def get_coop_levels(self):
        return self.cooperation_levels

    def get_actions(self):
        return self.actions

    def get_neighbour_coords(self, row, col):
        for nei_coords in self.neighbour_offsets:
            yield ((row + nei_coords[0]) % self.size, (col + nei_coords[1]) % self.size)

    def get_random_neighbours_coords(self, row, col):
        nei_coords = self.neighbour_offsets[randint(0, len(self.neighbour_offsets)-1)]
        return tuple(((row + nei_coords[0]) % self.size, (col + nei_coords[1]) % self.size))

    def run_rounds(self, rounds):
        for _ in range(rounds):
            self.run_round()

    def run_round(self):
        # Compute next payoffs
        next_payoffs = []
        for row in range(self.size):
            next_row = []
            for col in range(self.size):
                next_row.append(self.get_cell_payoff(row, col))
            next_payoffs.append(next_row)
        self.payoffs = next_payoffs

        # Compute next cell values
        next_actions = []
        for row in range(self.size):
            next_row = []
            for col in range(self.size):
                next_row.append(self.get_best_neighbour_action(row, col))
            next_actions.append(next_row)
        self.actions = next_actions

        # Compute cooperation level
        self.cooperation_levels.append(self.get_coop_level())

    def get_cell_payoff(self, row, col):
        payoff = 0
        cell_action = self.actions[row][col]
        for nei_row, nei_col in self.get_neighbour_coords(row, col):
            nei_action = self.actions[nei_row][nei_col]
            payoff += self.payoff_matrix[cell_action][nei_action]
        return payoff

    def get_best_neighbour_action(self, row, col):
        if self.pure_imitation:
            return self.get_pure_imitation(row, col)
        else:
            return self.get_by_replicator(row, col)

    def get_pure_imitation(self, row, col):
        best_payoff = self.payoffs[row][col]
        best_action = self.actions[row][col]
        for nei_row, nei_col in self.get_neighbour_coords(row, col):
            if self.payoffs[nei_row][nei_col] > best_payoff:
                best_payoff = self.payoffs[nei_row][nei_col]
                best_action = self.actions[nei_row][nei_col]
        return best_action

    def get_by_replicator(self, row, col):
        nei_row, nei_col = self.get_random_neighbours_coords(row, col)
        cell_payoff = self.get_cell_payoff(row, col)
        neighbour_payoff = self.get_cell_payoff(nei_row, nei_col)
        copy_probability = (1 + (cell_payoff-neighbour_payoff)/self.replicator_divisor)/2

        return self.actions[row][col] if copy_probability > random() else self.actions[nei_row][nei_col]

    def __repr__(self):
        result = "Payoffs :\n"
        result += "\n".join("  ".join(str(v) for v in row) for row in self.payoffs) + "\n"
        result += "Cells :\n"
        result += "\n".join("  ".join(str(v) for v in row) for row in self.actions) + "\n"
        result += "Coop level : \n"
        result += " ".join(str(v) for v in self.cooperation_levels) + "\n"
        return result


def display_table(actions, filename):
    plt.clf()
    color_map = ListedColormap(["r", "g"])
    plt.matshow(actions, cmap=color_map)
    plt.savefig("figures/boards/" + filename)


def display_coop_level(coop_level, filename):
    plt.clf()
    plt.plot(coop_level, color="b")
    plt.xlabel("Round")
    plt.ylabel("Cooperation level")
    plt.savefig("figures/coop/" + filename)


def display_coop_levels(coop_levels, filename):
    plt.clf()
    colors = {50:"b", 20:"r", 12:"g", 8:"m", 4:"k"}
    for size, coop_level in sorted(coop_levels.items(), key=itemgetter(0)):
        plt.plot(coop_level, label=str(size)+"x"+str(size), color=colors[size])
    plt.xlabel("Round")
    plt.ylabel("Cooperation level")
    plt.legend()
    plt.savefig("figures/coop-avg/" + filename)


def display_bar_plot(categories, filename):
    plt.clf()
    coop_values = categories["coop"]
    defect_values = categories["defect"]
    size_values = categories["sizes"]
    N = 5
    ind = np.arange(N)  # the x locations for the groups
    width = 0.35  # the width of the bars

    fig, ax = plt.subplots()
    coop_bar = ax.bar(ind, coop_values, width, color='g')
    defect_bar = ax.bar(ind + width, defect_values, width, color='r')

    # add some text for labels, title and axes ticks
    ax.set_xlabel('Board sizes')
    ax.set_ylabel('Percentage of games won')
    ax.set_xticks(ind + width / 2)
    ax.set_xticklabels(size_values)

    ax.legend((coop_bar[0], defect_bar[0]), ('Cooperators', 'Defectors'))

    plt.savefig("figures/coop-avg/" + filename)


# Boards
#payoff_matrix = [[0, 10], [0, 7]]  # Prisoners
payoff_matrix = [[0, 10], [3, 7]]  # Snowdrift
"""
for size in [50]:
    board = Board(size, payoff_matrix, diagonal_neighbours=True, pure_imitation=False)

    total_rounds = 0
    for round_cap in (0, 1, 5, 10, 20, 50):
        board.run_rounds(round_cap-total_rounds)
        total_rounds = round_cap
        display_table(board.get_actions(),
                      "moore-snowdrift-replicator-size %i-round %i" % (size, total_rounds))

    display_coop_level(board.get_coop_levels(),
                       "moore-snowdrift-replicator-size %i-coop level" % size)


"""
# Cooperation levels
coop_levels_sizes = {}
coop_levels_sizes_coop = {}
coop_levels_sizes_defect = {}
coop_levels_categ = {"coop":[], "defect":[], "sizes":[]}

max_rounds = 50
max_simulations = 50

for size in [50]:
    rounds_defect = 0
    moore_total_coop = [0 for i in range(max_rounds)]
    moore_total_defect = [0 for i in range(max_rounds)]

    for sim_count in range(max_simulations):
        print(size, sim_count)
        moore_board = Board(size, payoff_matrix, diagonal_neighbours=True, pure_imitation=False)
        moore_board.run_rounds(max_rounds)
        moore_coop = moore_board.get_coop_levels()
        if moore_coop[-1]>.5:
            moore_total_coop = [moore_total_coop[i] + moore_coop[i] for i in range(max_rounds)]
        else:
            moore_total_defect = [moore_total_defect[i] + moore_coop[i] for i in range(max_rounds)]
            rounds_defect += 1

    coop_levels_sizes[size] = [(moore_total_coop[i] + moore_total_defect[i]) / max_simulations for i in range(max_rounds)]

    coop_levels_categ["coop"].append((max_simulations - rounds_defect) / max_simulations)
    coop_levels_categ["defect"].append(rounds_defect / max_simulations)
    coop_levels_categ["sizes"].append(size)

    if max_simulations-rounds_defect>0:
        coop_levels_sizes_coop[size] = [moore_total_coop[i] / (max_simulations - rounds_defect) for i in range(max_rounds)]
    if rounds_defect>0:
        coop_levels_sizes_defect[size] = [moore_total_defect[i] / (rounds_defect) for i in range(max_rounds)]

#display_coop_levels(coop_levels_sizes_coop, "moore-snowdrift-replicator-all sizes-avg coop-coop win")
#display_coop_levels(coop_levels_sizes_defect, "moore-snowdrift-replicator-all sizes-avg coop-defect win")
#display_bar_plot(coop_levels_categ, "moore-snowdrift-replicator-all sizes-categories")
display_coop_levels({50:coop_levels_sizes[50]}, "moore-snowdrift-replicator-size 50-avg coop")
#display_coop_levels(coop_levels_sizes, "moore-snowdrift-replicator-all sizes-avg coop")