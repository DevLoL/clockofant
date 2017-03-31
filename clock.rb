#!/usr/bin ruby

require 'pp'

cols = 9
rows = 8

# SSSSS
# SMMMS
# SMHMS
# SMMMS
# SSSSS
clock_map = [
  [52, 54, 56, 58,  0,  2,  4,  6,  8],
  [50, 52, 55, 57,  0,  3,  5,  8, 10],
  [48, 49, 10, 11,  0,  1,  2, 11, 12],
  [46, 46,  9,  0,  0,  0,  3, 14, 14],
  [44, 44,  9,  0,  0,  0,  3, 16, 16],
  [42, 41,  8,  7,  6,  5,  4, 19, 18],
  [40, 38, 35, 33, 30, 27, 25, 22, 20],
  [38, 36, 34, 32, 30, 28, 26, 24, 22], 
]

def circle_to_rect(w, h, offset = 0, start = 0)
  vals = (
    (0...w).to_a.zip(w.times.collect{0}) + # top
    h.times.collect{w-1}.zip((0..h).to_a) + # right side
    (0..(w-1)).to_a.reverse.zip(w.times.collect{h-1}) +  # bottom
    h.times.collect{0}.zip(0..h).to_a.reverse
  ).uniq.reject{|k| k.any?(&:nil?) }
  if offset != 0
    vals.map do |k|
      k[0] += offset
      k[1] += offset
    end
  end
  if start > 0
    vals = vals[start..-1].concat(vals[0...start])
  end
  vals
end

puts circle_to_rect(5, 4, 2, 2)
puts circle_to_rect(7, 6, 1, 3)
puts circle_to_rect(9, 8)

=begin

hours = [[4, 2], [5, 2], [6, 2], [6, 3], [6, 4], [6, 5], [5, 5], [4, 5], [3, 5], [2, 5], [2, 4]. [2, 3], [2, 2], [3, 2]]
hours = [[4, 2], [5, 2], [6, 2], [6, 3], [6, 4], [6, 5], [5, 5], [4, 5], [3, 5], [2, 5], [2, 4], [2, 3], [2, 2], [3, 2]]
hours = (4..6).to_a.zip([2,2,2]) + [6,6,6].zip((3..5).to_a) + (2..5).to_a.reverse.zip([5,5,5,5]) + [2,2,2].zip((2..4).to_a.reverse) + (2...3).to_a.zip([2,2])
minutes = (4..7).to_a.zip([1,1,1,1]) + [7,7,7,7,7].zip((2..6).to_a) + (1..6).to_a.reverse.zip([6,6,6,6,6,6]) + [1,1,1,1,1].zip((1..5).to_a.reverse) + (1...5).to_a.zip([1,1,1,1])
[[4, 1], [5, 1], [6, 1], [7, 1], [7, 2], [7, 3], [7, 4], [7, 5], [7, 6], [6, 6], [5, 6], [4, 6], [3, 6], [2, 6], [1, 6], [1, 5], [1, 4], [1, 3], [1, 2], [1, 1], [1, 1], [2, 1], [3, 1], [4, 1]]
seconds = (4..8).to_a.zip([0,0,0,0,0]) + [8,8,8,8,8,8].zip((1..8).to_a) + (0..8).to_a.reverse.zip([7,7,7,7,7,7,7,7]) + [0,0,0,0,0,0].zip((0..6).to_a.reverse) + (0...6).to_a.zip([0,0,0,0,0,0])
seconds = [[4, 0], [5, 0], [6, 0], [7, 0], [8, 0], [8, 1], [8, 2], [8, 3], [8, 4], [8, 5], [8, 6], [7, 7], [6, 7], [5, 7], [4, 7], [3, 7], [2, 7], [1, 7], [0, 7], [0, 6], [0, 5], [0, 4], [0, 3], [0, 2], [0, 1], [0, 0], [1, 0], [2, 0], [3, 0], [4, 0], [5, 0]]
seconds = fill_map

hours_handle = []

=end
