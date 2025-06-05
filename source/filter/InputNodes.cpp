#include "../../include/filter/InputNodes.hpp"
#include <algorithm>
#include <wpi/SmallVector.h>
#include <wpi/SpanExtras.h>
#include <wpi/StringExtras.h>
#include <wpi/fs.h>

namespace filter {

LogFileInput::LogFileInput(const std::string& filename, const std::string& columnName)
    : filename_(filename), columnName_(columnName), connected_(false) {
    start();
}

bool LogFileInput::isConnected() const {
    return connected_;
}

std::vector<double> LogFileInput::getData() const {
    if (!connected_ || !reader_) {
        return {};
    }
    return data_;
}

void LogFileInput::start() {
    if (filename_.empty() || columnName_.empty()) {
        connected_ = false;
        return;
    }

    auto fileBuffer = wpi::MemoryBuffer::GetFile(filename_);
    if (!fileBuffer) {
        connected_ = false;
        return;
    }

    wpi::DataLogReader reader{std::move(*fileBuffer)};
    if (!reader.IsValid()) {
        connected_ = false;
        return;
    }

    reader_ = std::make_unique<wpi::DataLogReader>(std::move(reader));
    connected_ = true;

    // Load data for the specified column
    data_.clear();
    reader_->ForEachEntry([&](const wpi::DataLogReaderEntry& entry) {
        if (entry.name == columnName_ && entry.type == "double") {
            data_.push_back(entry.value);
        }
    });
}

void LogFileInput::stop() {
    reader_.reset();
    connected_ = false;
    data_.clear();
}

NetworkTableInput::NetworkTableInput(const std::string& tableName, const std::string& key)
    : tableName_(tableName), key_(key), useUSB_(true), teamNumber_(0), connected_(false) {
    start();
}

bool NetworkTableInput::isConnected() const {
    return connected_;
}

std::vector<double> NetworkTableInput::getData() const {
    return data_;
}

void NetworkTableInput::start() {
    // TODO: Implement NetworkTable connection
    connected_ = false;
}

void NetworkTableInput::stop() {
    connected_ = false;
    data_.clear();
}

void NetworkTableInput::setUseUSB(bool useUSB) {
    useUSB_ = useUSB;
}

void NetworkTableInput::setTeamNumber(int teamNumber) {
    teamNumber_ = teamNumber;
}

void NetworkTableInput::setIPAddress(const std::string& ipAddress) {
    ipAddress_ = ipAddress;
}

LogLoader::LogLoader() = default;

void LogLoader::Display() {
    if (ImGui::Button("Open data log file...")) {
        opener_ = std::make_unique<pfd::open_file>(
            "Select Data Log", "",
            std::vector<std::string>{"DataLog Files", "*.wpilog"});
    }

    // Handle opening the file
    if (opener_ && opener_->ready(0)) {
        if (!opener_->result().empty()) {
            filename_ = opener_->result()[0];

            auto fileBuffer = wpi::MemoryBuffer::GetFile(filename_);
            if (!fileBuffer) {
                ImGui::OpenPopup("Error");
                error_ = fmt::format("Could not open file: {}",
                                  fileBuffer.error().message());
                return;
            }

            wpi::DataLogReader reader{std::move(*fileBuffer)};
            if (!reader.IsValid()) {
                ImGui::OpenPopup("Error");
                error_ = "Not a valid datalog file";
                return;
            }
            reader_ = std::make_unique<wpi::DataLogReader>(std::move(reader));
            entryTree_.clear();
        }
        opener_.reset();
    }

    // Handle errors
    ImGui::SetNextWindowSize(ImVec2(480.f, 0.0f));
    if (ImGui::BeginPopupModal("Error")) {
        ImGui::PushTextWrapPos(0.0f);
        ImGui::TextUnformatted(error_.c_str());
        ImGui::PopTextWrapPos();
        if (ImGui::Button("Close")) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }

    if (!reader_) {
        return;
    }

    // Summary info
    ImGui::TextUnformatted(fs::path{filename_}.stem().string().c_str());
    ImGui::Text("%u records, %u entries", reader_->GetNumRecords(),
                reader_->GetNumEntries());

    bool refilter = ImGui::InputText("Filter", &filter_);

    // Display tree of entries
    if (entryTree_.empty() || refilter) {
        RebuildEntryTree();
    }

    ImGui::BeginTable(
        "Entries", 2,
        ImGuiTableFlags_Borders | ImGuiTableFlags_SizingStretchProp);
    ImGui::TableSetupColumn("Name");
    ImGui::TableSetupColumn("Type");
    ImGui::TableHeadersRow();
    DisplayEntryTree(entryTree_);
    ImGui::EndTable();
}

void LogLoader::RebuildEntryTree() {
    entryTree_.clear();
    wpi::SmallVector<std::string_view, 16> parts;
    reader_->ForEachEntry([&](const wpi::DataLogReaderEntry& entry) {
        // only show double/float/string entries
        if (entry.type != "double" && entry.type != "float" &&
            entry.type != "string") {
            return;
        }

        // filter on name
        if (!filter_.empty() && !wpi::contains_lower(entry.name, filter_)) {
            return;
        }

        parts.clear();
        // split on first : if one is present
        auto [prefix, mainpart] = wpi::split(entry.name, ':');
        if (mainpart.empty() || wpi::contains(prefix, '/')) {
            mainpart = entry.name;
        } else {
            parts.emplace_back(prefix);
        }
        wpi::split(mainpart, parts, '/', -1, false);

        // ignore a raw "/" key
        if (parts.empty()) {
            return;
        }

        // get to leaf
        auto nodes = &entryTree_;
        for (auto part : wpi::drop_back(std::span{parts.begin(), parts.end()})) {
            auto it =
                std::find_if(nodes->begin(), nodes->end(),
                           [&](const auto& node) { return node.name == part; });
            if (it == nodes->end()) {
                nodes->emplace_back(part);
                nodes->back().path.assign(
                    entry.name.data(), part.data() + part.size() - entry.name.data());
                it = nodes->end() - 1;
            }
            nodes = &it->children;
        }

        auto it = std::find_if(nodes->begin(), nodes->end(), [&](const auto& node) {
            return node.name == parts.back();
        });
        if (it == nodes->end()) {
            nodes->emplace_back(parts.back());
            it = nodes->end() - 1;
        }
        it->entry = &entry;
    });
}

void LogLoader::EmitEntry(const std::string& name,
                         const wpi::DataLogReaderEntry& entry) {
    ImGui::TableNextColumn();
    ImGui::Selectable(name.c_str());
    if (ImGui::BeginDragDropSource()) {
        auto entryPtr = &entry;
        ImGui::SetDragDropPayload(
            entry.type == "string" ? "DataLogEntryString" : "DataLogEntry",
            &entryPtr,
            sizeof(entryPtr));
        ImGui::TextUnformatted(entry.name.data(),
                             entry.name.data() + entry.name.size());
        ImGui::EndDragDropSource();
    }
    ImGui::TableNextColumn();
    ImGui::TextUnformatted(entry.type.data(),
                         entry.type.data() + entry.type.size());
}

void LogLoader::DisplayEntryTree(const std::vector<EntryTreeNode>& tree) {
    for (auto&& node : tree) {
        if (node.entry) {
            EmitEntry(node.name, *node.entry);
        }

        if (!node.children.empty()) {
            ImGui::TableNextColumn();
            bool open = ImGui::TreeNodeEx(node.name.c_str(),
                                        ImGuiTreeNodeFlags_SpanFullWidth);
            ImGui::TableNextColumn();
            if (open) {
                DisplayEntryTree(node.children);
                ImGui::TreePop();
            }
        }
    }
}

std::vector<double> LogLoader::GetData(const std::string& entryName) const {
    if (!reader_) {
        return {};
    }

    std::vector<double> data;
    reader_->ForEachEntry([&](const wpi::DataLogReaderEntry& entry) {
        if (entry.name == entryName && entry.type == "double") {
            data.push_back(entry.value);
        }
    });
    return data;
}

} // namespace filter
