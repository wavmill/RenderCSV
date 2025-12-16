/// @file  md.cpp
#include "md.hpp"
#include "table_formatter_utils.hpp"
#include <sstream>
#include <vector>
#include <string>
#include <cstddef>
#include <algorithm>

namespace render_csv
{

    // О формате GFM см.: https://github.github.com/gfm/
    [[nodiscard]] static auto formatMarkdownGfm(TableData const& data)
        -> TableFormatterResult
    {
        TableFormatterResult result;
        
        TableFormatterResult::Log validationLog = simpleValidate(data);
        result.warnings = validationLog;
        
        if (data.empty()) {
            return result;
        }

        std::ostringstream output;
        const std::size_t numRows = data.size();
        const std::size_t numCols = data[0].size();
        
       
        TableColumnWidths widths = computeColumnWidthsUtf8(data);
        std::vector<TableColumnWidths::ColumnWidth> maxWidths(numCols, 0);
        
        // Ширина заголовков 
        for (std::size_t col = 0; col < numCols && col < widths.headers.size(); ++col) {
            maxWidths[col] = std::max(maxWidths[col], widths.headers[col]);
        }
        
        // Ширина строк
        for (std::size_t bodyRow = 0; bodyRow < widths.body.size(); ++bodyRow) {
            for (std::size_t col = 0; col < numCols && col < widths.body[bodyRow].size(); ++col) {
                maxWidths[col] = std::max(maxWidths[col], widths.body[bodyRow][col]);
            }
        }

        //Заголовок таблицы 
        output << "|";
        for (std::size_t col = 0; col < numCols; ++col) {
            const std::string& cell = data[0][col];
            TableColumnWidths::ColumnWidth cellWidth = 0;
            if (col < widths.headers.size()) {
                cellWidth = widths.headers[col];
            }
            
            TableColumnWidths::ColumnWidth padding = maxWidths[col] - cellWidth;
            
            output << " " << cell;
            for (TableColumnWidths::ColumnWidth i = 0; i < padding; ++i) {
                output << " ";
            }
            output << " |";
        }
        output << "\n";

        output << "|";
        for (std::size_t col = 0; col < numCols; ++col) {
            output << " ";
            for (TableColumnWidths::ColumnWidth i = 0; i < maxWidths[col]; ++i) {
                output << "-";
            }
            output << " |";
        }
        output << "\n";

        for (std::size_t row = 1; row < numRows; ++row) {
            output << "|";
            
            for (std::size_t col = 0; col < numCols; ++col) {
                const std::string& cell = data[row][col];
                
                TableColumnWidths::ColumnWidth cellWidth = 0;
                std::size_t bodyRowIndex = row - 1;
                
                if (bodyRowIndex < widths.body.size() && col < widths.body[bodyRowIndex].size()) {
                    cellWidth = widths.body[bodyRowIndex][col];
                }
                
                TableColumnWidths::ColumnWidth padding = maxWidths[col] - cellWidth;
                
                output << " " << cell;
                for (TableColumnWidths::ColumnWidth i = 0; i < padding; ++i) {
                    output << " ";
                }
                output << " |";
            }
            
            if (row == numRows - 1) {
                bool isCaption = true;

std::string caption;
                
                for (std::size_t col = 0; col < numCols; ++col) {
                    if (col == 0) {
                        caption = data[row][col];
                    } else if (!data[row][col].empty()) {
                        isCaption = false;
                        break;
                    }
                }

                if (isCaption && !caption.empty()) {
                    output << "\n: " << caption;
                } else {
                    if (row < numRows - 1) {
                        output << "\n";
                    }
                }
            } else {
                output << "\n";
            }
        }

        result.output = output.str();
        return result;
    }

    auto makeMarkdownFormatter(MarkdownKind kind)
        -> TableFormatter
    {
        switch (kind) {
        case MarkdownKind::GithubFlavored:
            return formatMarkdownGfm;
        }
        
        return {};
    }

} // namespace render_csv