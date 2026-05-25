//
// Created by Volodymyr Avvakumov on 24.05.2026.
//

#ifndef ENGINE_POINTS_EXTRACTOR_H
#define ENGINE_POINTS_EXTRACTOR_H

#include <cstddef>
#include <tuple>
#include <vector>

std::vector<double> addColorsToPoints(
    const std::vector<double>& points,
    const std::vector<std::tuple<double, double, double>>& colors,
    size_t numbers_per_point=3
    );

std::vector<double> normalizePointPositions(
    const std::vector<double>& points,
    size_t numbers_per_point=3
);

#endif //ENGINE_POINTS_EXTRACTOR_H
