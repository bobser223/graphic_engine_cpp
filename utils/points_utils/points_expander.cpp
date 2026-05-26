//
// Created by Volodymyr Avvakumov on 24.05.2026.
//
#include "points_expander.h"

#include <algorithm>
#include <limits>

#include "Logger.h"


std::vector<double> addColorsToPoints(
    const std::vector<double>& points,
    const std::vector<std::tuple<double, double, double>>& colors,
    const size_t numbers_per_point
    ) {

    constexpr int values_per_color = 3;

    LOG_DEBUG(
        "Adding colors to points: points_values=", points.size(),
        ", colors=", colors.size(),
        ", numbers_per_point=", numbers_per_point
    );

    if (colors.empty()) {
        LOG_WARN("Cannot add colors: color list is empty");
        return points;
    }

    size_t point_count = points.size() / numbers_per_point;

    std::vector<double> result(point_count * (numbers_per_point + values_per_color));

    size_t j = 0;
    for (size_t i = 0; i < point_count; ++i) {

        size_t src = i * numbers_per_point;
        size_t dst = i * (numbers_per_point + values_per_color);

        result[dst]     = points[src];
        result[dst + 1] = points[src + 1];
        result[dst + 2] = points[src + 2];

        result[dst + 3] = std::get<0>(colors[j]);
        result[dst + 4] = std::get<1>(colors[j]);
        result[dst + 5] = std::get<2>(colors[j]);

        j++;
        j %= colors.size();
    }

    LOG_INFO("Colors added to ", point_count, " points");
    return result;
}

std::vector<double> normalizePointPositions(
    const std::vector<double>& points,
    const size_t numbers_per_point
) {
    constexpr size_t position_components = 3;

    if (points.empty() || numbers_per_point < position_components) {
        LOG_WARN(
            "Skipping point normalization: points_values=", points.size(),
            ", numbers_per_point=", numbers_per_point
        );
        return points;
    }

    double min_x = std::numeric_limits<double>::max();
    double min_y = std::numeric_limits<double>::max();
    double min_z = std::numeric_limits<double>::max();
    double max_x = -std::numeric_limits<double>::max();
    double max_y = -std::numeric_limits<double>::max();
    double max_z = -std::numeric_limits<double>::max();

    const size_t point_count = points.size() / numbers_per_point;

    for (size_t i = 0; i < point_count; ++i) {
        const size_t src = i * numbers_per_point;

        min_x = std::min(min_x, points[src]);
        min_y = std::min(min_y, points[src + 1]);
        min_z = std::min(min_z, points[src + 2]);

        max_x = std::max(max_x, points[src]);
        max_y = std::max(max_y, points[src + 1]);
        max_z = std::max(max_z, points[src + 2]);
    }

    const double width = max_x - min_x;
    const double height = max_y - min_y;
    const double depth = max_z - min_z;
    const double max_dimension = std::max({width, height, depth});

    if (max_dimension == 0.0) {
        LOG_WARN("Skipping point normalization: all points have the same position");
        return points;
    }

    std::vector<double> normalized = points;

    for (size_t i = 0; i < point_count; ++i) {
        const size_t dst = i * numbers_per_point;

        normalized[dst] = (normalized[dst] - min_x) / max_dimension;
        normalized[dst + 1] = (normalized[dst + 1] - min_y) / max_dimension;
        normalized[dst + 2] = (normalized[dst + 2] - min_z) / max_dimension;
    }

    LOG_INFO(
        "Normalized ", point_count,
        " points with bounds min=(",
        min_x, ", ", min_y, ", ", min_z,
        "), max=(",
        max_x, ", ", max_y, ", ", max_z,
        ")"
    );

    return normalized;
}
