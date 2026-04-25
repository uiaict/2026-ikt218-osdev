#include "kernel/paging.h"

#include <kernel/log.h>
#include <kernel/pmm.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>


// Fixed boot-time identity mappings.
// These entries ensure execution can continue immediately after paging is enabled:
// the CPU keeps fetching instructions at the current linear address, so the
// backing physical pages must still be reachable at the same virtual addresses.
#define IDENTITY_MAP_BASE_4M 0x400000

#define BOOT_PTE_104 0x104
#define BOOT_PTE_108 0x108
#define BOOT_PTE_109 0x109

#define PHYS_104_000 0x104000
#define PHYS_108_000 0x108000
#define PHYS_109_000 0x109000


#define USER_PGD_SLOT 32
static uint32_t* page_directory = NULL;
static uint32_t* page_directory_phys = NULL;

extern uint32_t _page_dir;
extern uint32_t _page_tables;

// Return page table for virt, allocating and zeroing one if the PDE is absent. The PDE stores a physical address +
// flags, masking removes flag bits
static uint32_t* get_or_create_pt(uint32_t virt) {
  uint32_t pgd_idx = PGD_INDEX(virt);
  if (page_directory[pgd_idx] & PAGE_PRESENT) {
    return (uint32_t*)(page_directory[pgd_idx] & PAGE_MASK); // clear flag bits to get 4 KiB aligned base address
  }
  uint32_t pt_phys = pmm_alloc_frame();
  if (!pt_phys) {
    printf("get_or_create_pt: failed to alloc PT for PD[%u]\n", pgd_idx);
    return NULL;
  }
  memset((void*)pt_phys, 0, PAGE_SIZE);
  page_directory[pgd_idx] = pt_phys | PAGE_KERNEL_RW;
  return (uint32_t*)pt_phys;
}

void paging_map_virtual_to_phys(uint32_t virt, uint32_t phys) {
  uint32_t* pt = get_or_create_pt(virt);
  if (!pt)
    return;
  uint32_t pte_idx = PTE_INDEX(virt);
  pt[pte_idx] = phys | PAGE_KERNEL_RW;
  // flush translation from TLB so later accesses can see updated mapping
  invalidate_page(virt);
}

uint32_t paging_get_phys(uint32_t virt) {
  uint32_t pgd_idx = PGD_INDEX(virt);
  uint32_t pte_idx = PTE_INDEX(virt);
  if (!(page_directory[pgd_idx] & PAGE_PRESENT)) {
    return 0;
  }
  uint32_t* pt = (uint32_t*)(page_directory[pgd_idx] & PAGE_MASK);
  if (!(pt[pte_idx] & PAGE_PRESENT)) {
    return 0;
  }
// Construct phys address from PTE base and virtual offset
  return (pt[pte_idx] & PAGE_MASK) + (virt & ~PAGE_MASK);
}

int vmm_map_page(uint32_t virt, uint32_t phys, uint32_t flags) {
  uint32_t* pt = get_or_create_pt(virt);
  if (!pt)
    return -1;
  uint32_t pte_idx = PTE_INDEX(virt);
  pt[pte_idx] = phys | flags;
  invalidate_page(virt);
  return 0;
}

int vmm_unmap_page(uint32_t virt) {
  uint32_t pgd_idx = PGD_INDEX(virt);
  uint32_t pte_idx = PTE_INDEX(virt);
  if (!(page_directory[pgd_idx] & PAGE_PRESENT)) {
    return 0;
  }
  uint32_t* pt = (uint32_t*)(page_directory[pgd_idx] & PAGE_MASK);
  pt[pte_idx] = 0;
  invalidate_page(virt);
  return 0;
}

int vmm_map_user_page(uint32_t virt, uint32_t phys, uint32_t flags) {
  uint32_t pgd_idx = PGD_INDEX(virt);
  if (!(page_directory[pgd_idx] & PAGE_PRESENT)) {
    uint32_t pt_phys = pmm_alloc_frame();
    if (!pt_phys) {
      printf("vmm_map_user_page: failed to alloc PT for PD[%u]\n", pgd_idx);
      return -1;
    }
    memset((void*)pt_phys, 0, PAGE_SIZE);
    page_directory[pgd_idx] = pt_phys | PAGE_USER_RW;
  }
  uint32_t* pt = (uint32_t*)(page_directory[pgd_idx] & PAGE_MASK);
  uint32_t pte_idx = PTE_INDEX(virt);
  pt[pte_idx] = phys | flags | PAGE_USER;
  invalidate_page(virt);
  return 0;
}

void init_paging(void* mb_info) {
  (void)mb_info;

  page_directory = &_page_dir;
  page_directory_phys = page_directory;

  for (int i = 0; i < PT_ENTRIES; i++) {
    page_directory[i] = 0 | PAGE_RW;
  }

  uint32_t* first_pt = &_page_tables;
  for (int i = 0; i < PT_ENTRIES; i++) {
    first_pt[i] = (i * PAGE_SIZE) | PAGE_KERNEL_RW;
  }
  page_directory[0] = ((uint32_t)first_pt) | PAGE_KERNEL_RW;

  first_pt[BOOT_PTE_104] = PHYS_104_000 | PAGE_KERNEL_RW;
  first_pt[BOOT_PTE_108] = PHYS_108_000 | PAGE_KERNEL_RW;
  first_pt[BOOT_PTE_109] = PHYS_109_000 | PAGE_KERNEL_RW;

  uint32_t* second_pt = &_page_tables + PT_ENTRIES;
  for (int i = 0; i < PT_ENTRIES; i++) {
    second_pt[i] = (IDENTITY_MAP_BASE_4M + i * PAGE_SIZE) | PAGE_KERNEL_RW;
  }
  page_directory[1] = ((uint32_t)second_pt) | PAGE_KERNEL_RW;

  uint32_t user_pt = pmm_alloc_frame();
  if (user_pt) {
    memset((void*)user_pt, 0, PAGE_SIZE);
    page_directory[USER_PGD_SLOT] = user_pt | PAGE_USER_RW;
  }

  load_cr3((uint32_t)page_directory_phys);
  enable_paging();

  log_info("Paging enabled successfully!\n");
}
