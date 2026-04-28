package main

import (
	"encoding/csv"
	"encoding/json"
	"flag"
	"fmt"
	"os"
	"os/exec"
	"path/filepath"
	"runtime"
	"sort"
	"strconv"
	"strings"
	"text/tabwriter"
)

type authorStats struct {
	lines   int
	commits map[string]bool
	files   map[string]bool
}

type fileInfo struct {
	path  string
	empty bool
}

type fileResult struct {
	stats map[string]*authorStats
}

type result struct {
	Name    string `json:"name"`
	Lines   int    `json:"lines"`
	Commits int    `json:"commits"`
	Files   int    `json:"files"`
}

var extensionsMap = make(map[string][]string)

func loadLanguages() {
	var data []byte
	var err error

	paths := []string{
		"configs/language_extensions.json",
		"../configs/language_extensions.json",
		"../../configs/language_extensions.json",
		"../../../configs/language_extensions.json",
		"../../../../configs/language_extensions.json",
	}

	if pwd := os.Getenv("PWD"); pwd != "" {
		paths = append(paths, filepath.Join(pwd, "../../configs/language_extensions.json"))
		paths = append(paths, filepath.Join(pwd, "../../../configs/language_extensions.json"))
	}

	for _, p := range paths {
		if data, err = os.ReadFile(p); err == nil {
			break
		}
	}

	if err != nil {
		return
	}

	var raw interface{}
	if err := json.Unmarshal(data, &raw); err == nil {
		switch v := raw.(type) {
		case map[string]interface{}:
			for key, val := range v {
				langName := strings.ToLower(key)
				switch arr := val.(type) {
				case []interface{}:
					for _, e := range arr {
						if extStr, ok := e.(string); ok {
							extensionsMap[langName] = append(extensionsMap[langName], strings.ToLower(extStr))
						}
					}
				case map[string]interface{}:
					if exts, ok := arr["extensions"].([]interface{}); ok {
						for _, e := range exts {
							if extStr, ok := e.(string); ok {
								extensionsMap[langName] = append(extensionsMap[langName], strings.ToLower(extStr))
							}
						}
					}
				}
			}
		case []interface{}:
			for _, item := range v {
				if obj, ok := item.(map[string]interface{}); ok {
					if nameObj, ok := obj["name"]; ok {
						if nameStr, ok := nameObj.(string); ok {
							langName := strings.ToLower(nameStr)
							if exts, ok := obj["extensions"].([]interface{}); ok {
								for _, e := range exts {
									if extStr, ok := e.(string); ok {
										extensionsMap[langName] = append(extensionsMap[langName], strings.ToLower(extStr))
									}
								}
							}
						}
					}
				}
			}
		}
	}
}

func matchGlob(patterns []string, file string) bool {
	for _, p := range patterns {
		m, err := filepath.Match(p, file)
		if err == nil && m {
			return true
		}
	}
	return false
}

func shouldProcess(file string, extRestricted bool, allowedExts map[string]bool, excludes []string, restricts []string) bool {
	if extRestricted {
		ext := strings.ToLower(filepath.Ext(file))
		if !allowedExts[ext] {
			return false
		}
	}
	if len(excludes) > 0 && matchGlob(excludes, file) {
		return false
	}
	if len(restricts) > 0 && !matchGlob(restricts, file) {
		return false
	}
	return true
}

func processFile(repo, rev, file string, useCommitter bool, empty bool) map[string]*authorStats {
	res := make(map[string]*authorStats)

	if empty {
		cmd := exec.Command("git", "log", "-1", "--format=%H%n%an%n%cn", rev, "--", file)
		cmd.Dir = repo
		out, err := cmd.Output()
		if err != nil {
			return res
		}
		lines := strings.Split(strings.TrimSpace(string(out)), "\n")
		if len(lines) >= 3 {
			hash := lines[0]
			name := lines[1]
			if useCommitter {
				name = lines[2]
			}
			res[name] = &authorStats{
				lines:   0,
				commits: map[string]bool{hash: true},
				files:   map[string]bool{file: true},
			}
		}
		return res
	}

	cmd := exec.Command("git", "blame", "--porcelain", rev, "--", file)
	cmd.Dir = repo
	out, err := cmd.Output()
	if err != nil {
		return res
	}

	commits := make(map[string]string)
	lines := strings.Split(string(out), "\n")
	currentHash := ""

	for _, line := range lines {
		if len(line) == 0 {
			continue
		}
		if line[0] == '\t' {
			name := commits[currentHash]
			if name != "" {
				if res[name] == nil {
					res[name] = &authorStats{
						commits: make(map[string]bool),
						files:   make(map[string]bool),
					}
				}
				res[name].lines++
				res[name].commits[currentHash] = true
				res[name].files[file] = true
			}
			continue
		}

		parts := strings.SplitN(line, " ", 2)
		if len(parts) == 0 {
			continue
		}

		if len(parts[0]) == 40 {
			currentHash = parts[0]
			continue
		}

		if useCommitter {
			if parts[0] == "committer" && len(parts) > 1 {
				commits[currentHash] = parts[1]
			}
		} else {
			if parts[0] == "author" && len(parts) > 1 {
				commits[currentHash] = parts[1]
			}
		}
	}

	return res
}

func main() {
	repository := flag.String("repository", ".", "")
	revision := flag.String("revision", "HEAD", "")
	orderBy := flag.String("order-by", "lines", "")
	useCommitter := flag.Bool("use-committer", false, "")
	format := flag.String("format", "tabular", "")
	extsFlag := flag.String("extensions", "", "")
	langsFlag := flag.String("languages", "", "")
	excludeFlag := flag.String("exclude", "", "")
	restrictFlag := flag.String("restrict-to", "", "")

	flag.Parse()

	if *orderBy != "lines" && *orderBy != "commits" && *orderBy != "files" {
		os.Exit(1)
	}
	if *format != "tabular" && *format != "csv" && *format != "json" && *format != "json-lines" {
		os.Exit(1)
	}

	loadLanguages()

	allowedExts := make(map[string]bool)
	extRestricted := false

	if *extsFlag != "" {
		extRestricted = true
		for _, ext := range strings.Split(*extsFlag, ",") {
			allowedExts[strings.ToLower(ext)] = true
		}
	}

	if *langsFlag != "" {
		extRestricted = true
		for _, lang := range strings.Split(*langsFlag, ",") {
			langExts, ok := extensionsMap[strings.ToLower(lang)]
			if ok {
				for _, ext := range langExts {
					allowedExts[strings.ToLower(ext)] = true
				}
			} else {
				fmt.Fprintf(os.Stderr, "Warning: unknown language %s\n", lang)
			}
		}
	}

	var excludes, restricts []string
	if *excludeFlag != "" {
		excludes = strings.Split(*excludeFlag, ",")
	}
	if *restrictFlag != "" {
		restricts = strings.Split(*restrictFlag, ",")
	}

	cmd := exec.Command("git", "ls-tree", "-r", "-l", *revision)
	cmd.Dir = *repository
	out, err := cmd.Output()
	if err != nil {
		os.Exit(1)
	}

	var filesToProcess []fileInfo
	for _, line := range strings.Split(string(out), "\n") {
		if line == "" {
			continue
		}
		parts := strings.SplitN(line, "\t", 2)
		if len(parts) != 2 {
			continue
		}
		path := parts[1]
		fields := strings.Fields(parts[0])
		if len(fields) < 4 {
			continue
		}
		sizeStr := fields[3]
		if sizeStr == "-" {
			continue
		}
		size, err := strconv.ParseInt(sizeStr, 10, 64)
		if err != nil {
			continue
		}

		if shouldProcess(path, extRestricted, allowedExts, excludes, restricts) {
			filesToProcess = append(filesToProcess, fileInfo{path: path, empty: size == 0})
		}
	}

	numWorkers := runtime.NumCPU()
	jobs := make(chan fileInfo, len(filesToProcess))
	resultsChan := make(chan fileResult, len(filesToProcess))

	for w := 0; w < numWorkers; w++ {
		go func() {
			for job := range jobs {
				res := processFile(*repository, *revision, job.path, *useCommitter, job.empty)
				resultsChan <- fileResult{stats: res}
			}
		}()
	}

	for _, f := range filesToProcess {
		jobs <- f
	}
	close(jobs)

	globalStats := make(map[string]*authorStats)
	for i := 0; i < len(filesToProcess); i++ {
		res := <-resultsChan
		for author, st := range res.stats {
			if globalStats[author] == nil {
				globalStats[author] = &authorStats{
					commits: make(map[string]bool),
					files:   make(map[string]bool),
				}
			}
			globalStats[author].lines += st.lines
			for c := range st.commits {
				globalStats[author].commits[c] = true
			}
			for f := range st.files {
				globalStats[author].files[f] = true
			}
		}
	}

	// Инициализируем пустой срез, чтобы JSON кодер не вывел null при отсутствии результатов
	results := make([]result, 0)
	for name, st := range globalStats {
		results = append(results, result{
			Name:    name,
			Lines:   st.lines,
			Commits: len(st.commits),
			Files:   len(st.files),
		})
	}

	sort.Slice(results, func(i, j int) bool {
		r1, r2 := results[i], results[j]
		var keys1, keys2 []int

		switch *orderBy {
		case "lines":
			keys1 = []int{r1.Lines, r1.Commits, r1.Files}
			keys2 = []int{r2.Lines, r2.Commits, r2.Files}
		case "commits":
			keys1 = []int{r1.Commits, r1.Lines, r1.Files}
			keys2 = []int{r2.Commits, r2.Lines, r2.Files}
		default:
			keys1 = []int{r1.Files, r1.Lines, r1.Commits}
			keys2 = []int{r2.Files, r2.Lines, r2.Commits}
		}

		for k := 0; k < 3; k++ {
			if keys1[k] != keys2[k] {
				return keys1[k] > keys2[k]
			}
		}
		return r1.Name < r2.Name
	})

	switch *format {
	case "tabular":
		w := tabwriter.NewWriter(os.Stdout, 0, 0, 1, ' ', 0)
		fmt.Fprintln(w, "Name\tLines\tCommits\tFiles")
		for _, r := range results {
			fmt.Fprintf(w, "%s\t%d\t%d\t%d\n", r.Name, r.Lines, r.Commits, r.Files)
		}
		w.Flush()
	case "csv":
		w := csv.NewWriter(os.Stdout)
		w.Write([]string{"Name", "Lines", "Commits", "Files"})
		for _, r := range results {
			w.Write([]string{r.Name, strconv.Itoa(r.Lines), strconv.Itoa(r.Commits), strconv.Itoa(r.Files)})
		}
		w.Flush()
	case "json":
		enc := json.NewEncoder(os.Stdout)
		enc.Encode(results)
	case "json-lines":
		enc := json.NewEncoder(os.Stdout)
		for _, r := range results {
			enc.Encode(r)
		}
	}
}
