#include <iostream>
#include <chrono>
#include <iomanip>
#include <cstring>
#include "benchmark.h"
#include "table.h"
#include "btree.h"
#include "compiler.h"

void print_storage_and_alignment_metrics() {
    uint32_t used_bytes = LEAF_NODE_HEADER_SIZE + (LEAF_NODE_MAX_CELLS * LEAF_NODE_CELL_SIZE);
    double utilization = (static_cast<double>(used_bytes) / PAGE_SIZE) * 100.0;

    std::cout << "\n=== C++ DATABASE STORAGE & ALIGNMENT METRICS ===\n";
    std::cout << "OS Page Size (bytes):          " << PAGE_SIZE << "\n";
    std::cout << "Compiler sizeof(Row) (bytes):  " << sizeof(Row) << "\n";
    std::cout << "Serialized ROW_SIZE (bytes):   " << ROW_SIZE << "\n";
    std::cout << "Bytes Saved Per Row:           " << (sizeof(Row) - ROW_SIZE) << "\n";
    std::cout << "Common + Leaf Header (bytes):  " << LEAF_NODE_HEADER_SIZE << "\n";
    std::cout << "Leaf Cell Size (Key + Row):    " << LEAF_NODE_CELL_SIZE << "\n";
    std::cout << "Max Cells Per 4KB Leaf Page:   " << LEAF_NODE_MAX_CELLS << "\n";
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "4KB Leaf Page Utilization:     " << used_bytes << " / " << PAGE_SIZE 
              << " bytes (" << utilization << "%)\n";
    std::cout << "Max Pager Capacity:            " << TABLE_MAX_PAGES << " pages (" 
              << (TABLE_MAX_PAGES * PAGE_SIZE) / 1024 << " KB)\n";
    std::cout << "================================================\n\n";
}

void run_database_benchmarks(Table* table) {
    const int ITERATIONS = 100000;
    std::cout << "\n=== RUNNING IN-MEMORY ENGINE BENCHMARK (" << ITERATIONS << " OPS) ===\n";

    void* root_node = get_page(table->pager, table->root_page_num);

    char backup_page[PAGE_SIZE];
    std::memcpy(backup_page, root_node, PAGE_SIZE);

    initialize_leaf_node(root_node);
    for (uint32_t i = 1; i <= LEAF_NODE_MAX_CELLS - 1; ++i) {
        Row r{};
        r.id = i * 2;
        std::snprintf(r.username, COLUMN_USERNAME_SIZE + 1, "user_%u", i);
        std::snprintf(r.email, COLUMN_EMAIL_SIZE + 1, "user_%u@example.com", i);
        leaf_node_insert(root_node, i - 1, r.id, &r);
    }

    
    Statement stmt;
    std::string sample_query = "insert 13 bench_user bench_user@example.com";
    auto compile_start = std::chrono::steady_clock::now();
    for (int i = 0; i < ITERATIONS; ++i) {
        prepare_statement(sample_query, &stmt);
    }
    auto compile_end = std::chrono::steady_clock::now();
    long long total_compile_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(
        compile_end - compile_start
    ).count();

    Row fetched_row{};
    volatile uint32_t checksum = 0;
    auto search_start = std::chrono::steady_clock::now();
    for (int i = 0; i < ITERATIONS; ++i) {
        uint32_t target_key = ((i % 12) + 1) * 2;
        Cursor* cursor = table_find(table, target_key);
        deserialize_row(cursor_value(cursor), &fetched_row);
        checksum += fetched_row.id;
        delete cursor;
    }
    auto search_end = std::chrono::steady_clock::now();
    long long total_search_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(
        search_end - search_start
    ).count();

    Row test_row{};
    test_row.id = 13;
    std::snprintf(test_row.username, COLUMN_USERNAME_SIZE + 1, "bench");
    std::snprintf(test_row.email, COLUMN_EMAIL_SIZE + 1, "bench@example.com");

    auto write_start = std::chrono::steady_clock::now();
    for (int i = 0; i < ITERATIONS; ++i) {
        Cursor* c_ins = table_find(table, test_row.id);
        leaf_node_insert(root_node, c_ins->cell_num, test_row.id, &test_row);
        delete c_ins;

        Cursor* c_del = table_find(table, test_row.id);
        leaf_node_delete(root_node, c_del->cell_num);
        delete c_del;
    }
    auto write_end = std::chrono::steady_clock::now();
    long long total_write_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(
        write_end - write_start
    ).count();

    std::memcpy(root_node, backup_page, PAGE_SIZE);

    double avg_compile_ns = static_cast<double>(total_compile_ns) / ITERATIONS;
    double avg_search_ns = static_cast<double>(total_search_ns) / ITERATIONS;
    double avg_write_ns = static_cast<double>(total_write_ns) / (ITERATIONS * 2.0); // 1 insert + 1 delete per loop
    double read_throughput = ITERATIONS / (total_search_ns / 1e9);
    double write_throughput = (ITERATIONS * 2.0) / (total_write_ns / 1e9);

    std::cout << std::fixed << std::setprecision(2);
    std::cout << "Avg SQL Compiler Parse Time:          " << avg_compile_ns << " ns (" 
              << (avg_compile_ns / 1000.0) << " us)\n";
    std::cout << "Avg O(log N) Lookup + Deserialize:    " << avg_search_ns << " ns (" 
              << (avg_search_ns / 1000.0) << " us)\n";
    std::cout << "Avg B-Tree Insert/Delete + Memcpy:    " << avg_write_ns << " ns (" 
              << (avg_write_ns / 1000.0) << " us)\n";
    std::cout << "Indexed Read Throughput:              " << read_throughput << " queries/sec\n";
    std::cout << "B-Tree Mutation Throughput:           " << write_throughput << " ops/sec\n";
    std::cout << "==============================================================\n\n";
}