// Local database - 8 programming topics with resources
const localDatabase = [
    {
        keyword: "JavaScript",
        pdf: "https://eloquentjavascript.net/Eloquent_JavaScript.pdf",
        gfg: "https://www.geeksforgeeks.org/javascript/",
        wiki: "https://en.wikipedia.org/wiki/JavaScript",
        yt: "https://www.youtube.com/results?search_query=javascript+tutorial"
    },
    {
        keyword: "Python",
        pdf: "https://www.py4e.com/book.php",
        gfg: "https://www.geeksforgeeks.org/python-programming-language/",
        wiki: "https://en.wikipedia.org/wiki/Python_(programming_language)",
        yt: "https://www.youtube.com/results?search_query=python+programming+tutorial"
    },
    {
        keyword: "React",
        pdf: "https://reactjs.org/docs/getting-started.html",
        gfg: "https://www.geeksforgeeks.org/react-js-introduction/",
        wiki: "https://en.wikipedia.org/wiki/React_(JavaScript_library)",
        yt: "https://www.youtube.com/results?search_query=react+js+tutorial"
    },
    {
        keyword: "Node.js",
        pdf: "https://nodejs.org/en/docs/",
        gfg: "https://www.geeksforgeeks.org/nodejs/",
        wiki: "https://en.wikipedia.org/wiki/Node.js",
        yt: "https://www.youtube.com/results?search_query=nodejs+tutorial"
    },
    {
        keyword: "Data Structures",
        pdf: "https://opendatastructures.org/ods-java.pdf",
        gfg: "https://www.geeksforgeeks.org/data-structures/",
        wiki: "https://en.wikipedia.org/wiki/Data_structure",
        yt: "https://www.youtube.com/results?search_query=data+structures+algorithms"
    },
    {
        keyword: "Algorithms",
        pdf: "https://mitpress.mit.edu/books/introduction-algorithms-third-edition",
        gfg: "https://www.geeksforgeeks.org/fundamentals-of-algorithms/",
        wiki: "https://en.wikipedia.org/wiki/Algorithm",
        yt: "https://www.youtube.com/results?search_query=algorithms+tutorial"
    },
    {
        keyword: "Machine Learning",
        pdf: "https://www.statlearning.com/",
        gfg: "https://www.geeksforgeeks.org/machine-learning/",
        wiki: "https://en.wikipedia.org/wiki/Machine_learning",
        yt: "https://www.youtube.com/results?search_query=machine+learning+tutorial"
    },
    {
        keyword: "Web Development",
        pdf: "https://developer.mozilla.org/en-US/docs/Learn",
        gfg: "https://www.geeksforgeeks.org/web-development/",
        wiki: "https://en.wikipedia.org/wiki/Web_development",
        yt: "https://www.youtube.com/results?search_query=web+development+tutorial"
    }
];

// DOM elements
const searchInput = document.getElementById('searchInput');
const searchBtn = document.getElementById('searchBtn');
const resultsSection = document.getElementById('resultsSection');
const resultsContainer = document.getElementById('resultsContainer');
const resultsTitle = document.getElementById('resultsTitle');
const resultsCount = document.getElementById('resultsCount');
const loading = document.getElementById('loading');
const suggestions = document.getElementById('suggestions');

// Search functionality
function performSearch(query) {
    if (!query.trim()) return;

    showLoading(true);
    hideResults();

    // Simulate API delay for better UX
    setTimeout(() => {
        const results = searchLocal(query);
        displayResults(results, query);
        showLoading(false);
    }, 500);
}

// Local search function
function searchLocal(query) {
    const normalizedQuery = query.toLowerCase().trim();
    return localDatabase.filter(entry => 
        entry.keyword.toLowerCase().includes(normalizedQuery)
    );
}

// Display search results
function displayResults(results, query) {
    resultsTitle.textContent = `Results for "${query}"`;
    resultsCount.textContent = `${results.length} result${results.length !== 1 ? 's' : ''} found`;
    
    if (results.length === 0) {
        resultsContainer.innerHTML = `
            <div class="no-results">
                <i class="fas fa-search"></i>
                <h3>No results found</h3>
                <p>Try searching for: JavaScript, Python, React, Node.js, Data Structures, Algorithms, Machine Learning, or Web Development</p>
            </div>
        `;
    } else {
        resultsContainer.innerHTML = results.map(result => createResultCard(result)).join('');
    }
    
    showResults();
    hideSuggestions();
}

// Create result card HTML
function createResultCard(result) {
    return `
        <div class="result-card">
            <div class="result-keyword">${result.keyword}</div>
            <div class="result-links">
                <a href="${result.pdf}" target="_blank" rel="noopener noreferrer" class="result-link pdf-link">
                    <i class="fas fa-file-pdf"></i>
                    PDF Resources
                </a>
                <a href="${result.gfg}" target="_blank" rel="noopener noreferrer" class="result-link gfg-link">
                    <i class="fas fa-code"></i>
                    GeeksforGeeks
                </a>
                <a href="${result.wiki}" target="_blank" rel="noopener noreferrer" class="result-link wiki-link">
                    <i class="fab fa-wikipedia-w"></i>
                    Wikipedia
                </a>
                <a href="${result.yt}" target="_blank" rel="noopener noreferrer" class="result-link yt-link">
                    <i class="fab fa-youtube"></i>
                    YouTube
                </a>
            </div>
        </div>
    `;
}

// Show/hide loading state
function showLoading(show) {
    loading.style.display = show ? 'block' : 'none';
}

// Show/hide results
function showResults() {
    resultsSection.style.display = 'block';
    resultsSection.scrollIntoView({ behavior: 'smooth', block: 'start' });
}

function hideResults() {
    resultsSection.style.display = 'none';
}

// Suggestions functionality
function showSuggestions(query) {
    if (!query.trim()) {
        hideSuggestions();
        return;
    }

    const normalizedQuery = query.toLowerCase();
    const matchingSuggestions = localDatabase
        .filter(entry => entry.keyword.toLowerCase().includes(normalizedQuery))
        .slice(0, 5);

    if (matchingSuggestions.length > 0) {
        suggestions.innerHTML = matchingSuggestions
            .map(entry => `
                <div class="suggestion-item" onclick="selectSuggestion('${entry.keyword}')">
                    ${entry.keyword}
                </div>
            `).join('');
        suggestions.style.display = 'block';
    } else {
        hideSuggestions();
    }
}

function hideSuggestions() {
    suggestions.style.display = 'none';
}

function selectSuggestion(keyword) {
    searchInput.value = keyword;
    hideSuggestions();
    performSearch(keyword);
}

// Popular topic search
function searchTopic(topic) {
    searchInput.value = topic;
    performSearch(topic);
}

// Event listeners
searchBtn.addEventListener('click', () => {
    performSearch(searchInput.value);
});

searchInput.addEventListener('keydown', (e) => {
    if (e.key === 'Enter') {
        performSearch(searchInput.value);
    }
});

searchInput.addEventListener('input', (e) => {
    showSuggestions(e.target.value);
});

// Click outside to hide suggestions
document.addEventListener('click', (e) => {
    if (!e.target.closest('.search-container')) {
        hideSuggestions();
    }
});

// Initialize with focus on search input
searchInput.focus();

// Add some smooth animations
document.addEventListener('DOMContentLoaded', () => {
    // Animate elements on load
    const header = document.querySelector('.header');
    const searchSection = document.querySelector('.search-section');
    const popularTopics = document.querySelector('.popular-topics');
    
    header.style.opacity = '0';
    header.style.transform = 'translateY(-20px)';
    
    searchSection.style.opacity = '0';
    searchSection.style.transform = 'translateY(20px)';
    
    popularTopics.style.opacity = '0';
    popularTopics.style.transform = 'translateY(20px)';
    
    setTimeout(() => {
        header.style.transition = 'all 0.8s ease';
        header.style.opacity = '1';
        header.style.transform = 'translateY(0)';
    }, 100);
    
    setTimeout(() => {
        searchSection.style.transition = 'all 0.8s ease';
        searchSection.style.opacity = '1';
        searchSection.style.transform = 'translateY(0)';
    }, 300);
    
    setTimeout(() => {
        popularTopics.style.transition = 'all 0.8s ease';
        popularTopics.style.opacity = '1';
        popularTopics.style.transform = 'translateY(0)';
    }, 500);
});